#include "TcpListener.h"
#include <fstream>
#include <sqlite3.h>
#include <unicode/unistr.h>
#include <unicode/regex.h>
#include <unicode/brkiter.h>
#include "CurlFetcher.cpp"
#include "LcsFlecter.h"

class OcsServer : public TcpListener {
    public:
        OcsServer(const char *ipAddress, int port, bool show_output, const char* db_path) : TcpListener(ipAddress, port), m_DB_path{db_path}, m_show_output{show_output}, m_dict_cookies{""} {
            if(!m_show_output) std::cout.setstate(std::ios_base::failbit);
            //this setting of the sozdik.kz cookies should be moved to only run when the language is set to Kazakh, when I implement language-separation
            std::ifstream kaz_cookies_file;
            kaz_cookies_file.open("kaz_cookies.txt");
            if(kaz_cookies_file.good()) {
                std::getline(kaz_cookies_file, m_dict_cookies);
                kaz_cookies_file.close();
            }
            else {
                std::cout << "No kaz_cookies.txt file found\n";
            }

            //printf("%s\n", m_DB_path);
        }

    protected:
        //virtual void onClientConnected(int clientSocket); //these two need to be gone from the OcsServer class in order to make the std::thread.detach() work in the TcpListener class
        //virtual void onClientDisconnected(int clientSocket);
        virtual void onMessageReceived(int clientSocket, const char *msg, int length);

    private:
        int getRequestType(const char* msg);     
        
        //bool c_strStartsWith(const char* str1, const char* str2);
        bool c_strStartsWith(const char *haystack, const char *needle);
        int c_strFind(const char* haystack, const char* needle);
        int c_strFindNearest(const char* haystack, const char* needle1, const char* needle2);

        int safeStrToInt(const std::string &string_number, int default_result=1);
        sqlite_int64 safeStrToInt64(const std::string &string_number, sqlite3_int64 default_result=1);

        void buildGETContent(short int page_type, char* url_c_str, std::string &content, bool cookies_present);
        void insertTextSelect(std::ostringstream &html);
        void sendBinaryFile(char* url_c_str, int clientSocket, const std::string &content_type);
        
        int checkHeaderEnd(const char* msg);
        void buildPOSTedData(const char* msg, bool headers_present, int length);
        void setURL(const char* msg);
        int getPostFields(const char* url);
        void handlePOSTedData(const char* post_data, int clientSocket);
        bool readCookie(std::string cookie[3], const char* msg);
        void setCookies(std::ostringstream &http_response_ss);

        bool lemmaTooltips(std::string _POST[1], int clientSocket);

        void writeSearchResults(const std::vector<int>& result_toknos_vec, sqlite3_stmt* statement1, sqlite3_stmt* statement2, std::ostringstream& text_results, std::ostringstream& lcs_results, std::ostringstream& tokno_results);
        void trigramSearch(sqlite3* db_connection, const std::string& trigram_table, const std::string& target_column, std::string query, int tokno_start, int tokno_end, int clientSocket);
        
        bool lcsSearch(std::string _POST[3], int clientSocket);
        bool lcsRegexSearch(std::string _POST[3], int clientSocket);
        bool lcsTrigramSearch(std::string _POST[3], int clientSocket);
        bool cyrFuzzySearch(std::string _POST[3], int clientSocket);
        bool cyrSearch(std::string _POST[3], int clientSocket);
        
        bool retrieveText(std::string text_id[1], int clientSocket);
        bool retrieveTextSubtitle(std::string text_id[2], int clientSocket);
        bool retrieveTextPageNo(std::string text_id[2], int clientSocket);
        void void_retrieveText(std::ostringstream &html);

        bool gorazdLookup(std::string _POST[2], int clientSocket);
        std::string curlGorazd(std::string plain_query, std::string uri_encoded_query);
        std::string curlGorazdThreads(std::string plain_query, std::string uri_encoded_query);

        bool curlLookup(std::string _POST[1], int clientSocket);

        std::string URIDecode(std::string &text);
        std::string htmlspecialchars(const std::string &innerHTML);
        std::string escapeQuotes(const std::string &quoteystring);
        std::string applySuperscripts(const std::string &database_text);

        void writeTextBody(std::ostringstream& html, sqlite3* DB, int tokno_start, int tokno_end);

        bool retrieveTextFromSearch(std::string _POST[1], int clientSocket);
        bool greekTooltips(std::string _POST[2], int clientSocket);

        bool generateInflection(std::string _POST[3], int clientSocket); 

        const char*         m_post_data;
        std::string         m_post_data_incomplete;
        int                 m_total_post_bytes;
        int                 m_bytes_handled;
        bool                m_POST_continue;
        char                m_url[50]; //only applies to POST urls
        const char*         m_DB_path;
        //these global cookies-related member-variables are not safe for the multithreaded TcpListener, because 2 incoming GET requests with different cookie-values will totally fuck up the reading and setting of these values. I need an actual proper strategy for making the server multi-threaded that isn't completely retarded
        std::string         m_cookies[3] {"1", "1", "1"};
        std::string         m_cookie_names[3] {"text_id", "current_pageno", "subtitle_id"};
        bool                m_show_output;

        std::string         m_dict_cookies;
        
        std::string         m_page_toknos_arr {"[]"};
        int                 m_pageno_count;
        int m_sentences_per_page {30};
};
