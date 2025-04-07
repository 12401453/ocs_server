#include "TcpListener_threads.h"
#include <fstream>
#include <sqlite3.h>
#include <unicode/unistr.h>
#include <unicode/regex.h>
#include <unicode/brkiter.h>
#include "CurlFetcher.cpp"

class OcsServer : public TcpListener {
    public:
        OcsServer(const char *ipAddress, int port, bool show_output) : TcpListener(ipAddress, port), m_DB_path{"Kazakh.db"}, m_show_output{show_output}, m_dict_cookies{""} {
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
        }

    protected:
        //virtual void onClientConnected(int clientSocket); //these two need to be gone from the OcsServer class in order to make the std::thread.detach() work in the TcpListener class
        //virtual void onClientDisconnected(int clientSocket);
        virtual void onMessageReceived(int clientSocket, const char *msg, int length, int get_or_post, int headers_length, int content_length=0);

    private:
        int getRequestType(const char* msg);     
        
        // bool c_strStartsWith(const char* str1, const char* str2);
        // int c_strFind(const char* haystack, const char* needle);
        void getURL(const char* msg, char* url);
        int c_strFindNearest(const char* haystack, const char* needle1, const char* needle2);

        int safeStrToInt(const std::string &string_number, int default_result=1);
        sqlite_int64 safeStrToInt64(const std::string &string_number, sqlite3_int64 default_result=1);

        void buildGETContent(short int page_type, char* url_c_str, std::string &content, bool cookies_present);
        void insertTextSelect(std::ostringstream &html);
        void insertLangSelect(std::ostringstream &html);
        void sendBinaryFile(char* url_c_str, int clientSocket, const std::string &content_type);
        
        int checkHeaderEnd(const char* msg);
        void buildPOSTedData(const char* msg, bool headers_present, int length);
        void setURL(const char* msg);
        int getPostFields(const char* url);
        void handlePOSTedData(const char* post_data, int clientSocket, const char* url);
        bool readCookie(std::string cookie[3], const char* msg);
        void setCookies(std::ostringstream &http_response_ss);

        bool addText(std::string _POST[3], int clientSocket);
        bool addTextOldEnglish(std::string _POST[3], int clientSocket);
        bool lemmaTooltips(std::string _POST[1], int clientSocket);
        bool lcsSearch(std::string _POST[3], int clientSocket);
        bool lcsRegexSearch(std::string _POST[3], int clientSocket);
        
        
        bool lemmaTooltipsMW(std::string _POST[3], int clientSocket);
        bool retrieveText(std::string text_id[1], int clientSocket);
        bool retrieveTextSubtitle(std::string text_id[2], int clientSocket);
        bool retrieveTextPageNo(std::string text_id[2], int clientSocket);
        void void_retrieveText(std::ostringstream &html);
        bool retrieveTextSplitup(std::string _POST[3], int clientSocket);
        bool retrieveEngword(std::string _POST[3], int clientSocket);
        bool recordLemma(std::string _POST[8], int clientSocket);
        bool deleteLemma(std::string _POST[3], int clientSocket);
        bool recordMultiwordOld(std::string _POST[8], int clientSocket);
        bool recordMultiword(std::string _POST[8], int clientSocket);
        bool deleteMultiword(std::string _POST[4], int clientSocket);
        bool updateMultiwordTranslations(std::string _POST[3], int clientSocket);
        bool deleteText(std::string _POST[1], int clientSocket);
        bool getLangId(std::string text_id[1], int clientSocket);
        bool retrieveMultiword(std::string _POST[3], int clientSocket);
        bool clearTable(int clientSocket);
        bool disregardWord(std::string _POST[2], int clientSocket);
        bool updateDisplayWord(std::string _POST[5], int clientSocket);

        bool pullInLemma(std::string _POST[4], int clientSocket);
        bool retrieveMeanings(std::string _POST[2], int clientSocket);
        bool pullInMultiword(std::string _POST[2], int clientSocket);
        bool retrieveMultiwordMeanings(std::string _POST[2], int clientSocket);
        bool pullInMultiwordByForm(std::string _POST[4], int clientSocket);

        bool curlLookup(std::string _POST[1], int clientSocket);
        bool curlLookup_sozdik(std::string _POST[1], int clientSocket);

        bool dumpLemmaTable(std::string _POST[1], int clientSocket);

        std::string URIDecode(std::string &text);
        std::string htmlspecialchars(const std::string &innerHTML);
        std::string escapeQuotes(const std::string &quoteystring);
        std::string applySuperscripts(const std::string &database_text);

        void writeTextBody(std::ostringstream& html, sqlite3* DB, int tokno_start, int tokno_end);

        bool retrieveTextFromSearch(std::string _POST[1], int clientSocket);
        bool greekTooltips(std::string _POST[2], int clientSocket);

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
