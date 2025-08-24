//compile with
//  g++ TcpListener.cpp OcsServer.cpp main.cpp -lsqlite3 -licuuc -licudata -licui18n -lcurl -o webserv
//run with
//  ./webserv <portnumber> [silent]
#include <sstream>
#include "OcsServer.h"
#include <math.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <unordered_set>

void OcsServer::onMessageReceived(int clientSocket, const char* msg, int length) {


    #define GET 3
    #define POST 4
  
    m_POST_continue = true;

    std::cout << "\n--------------------------------------MESSAGE BEGIN--------------------------------------------------\n\n" << msg;
    std::cout << "\n-----------------------------------------------------------------------------------------------------\n";
    std::cout << "Size of the above message is " << length << " bytes\n\n";
  
    int lb_pos = checkHeaderEnd(msg);
    int get_or_post = getRequestType(msg);
  
    std::cout << get_or_post << std::endl;

    if(get_or_post == -1 && lb_pos == -1) {
        buildPOSTedData(msg, false, length);

        if(m_POST_continue == false) {
            handlePOSTedData(m_post_data, clientSocket);
        }
    }

    else if (get_or_post == GET)
    {
        std::cout << "This is a GET request" << std::endl;

        std::cout << "lb_pos: " << lb_pos << std::endl;
        char msg_url[lb_pos - 12];
        std::cout << "size of msg_url array: " << sizeof(msg_url) << std::endl;

        for (int i = 4; i < lb_pos - 9; i++)
        {
            msg_url[i - 4] = msg[i];
            std::cout << msg[i];
        }
        std::cout << "\n";
        msg_url[lb_pos - 13] = '\0';

        short int page_type = 0;
        if(!strcmp(msg_url, "/texts")) page_type = 1;
        else if(!strcmp(msg_url, "/add_texts")) page_type = 2;
        else if(!strcmp(msg_url, "/words")) page_type = 3;

        //#include "docRoot.cpp"
   
        int url_size = strlen("HTML_DOCS") + sizeof(msg_url); //sizeof() can be used for c-strings declared as an array of char's but strlen() must be used for char *pointers
        char url_c_str[url_size];
    
        strcpy(url_c_str, "HTML_DOCS");
        strcat(url_c_str, msg_url);

        std::cout << "page_type: " << page_type << "\n";

        bool cookies_present = false;
        Cookies l_cookies;
        if(page_type > 0) cookies_present = readCookie(msg, l_cookies); //want to avoid reading cookies when serving auxilliary files like stylesheets
        std::cout << "Cookies present? " << cookies_present << "\n";
        if(cookies_present) {
            std::cout << "text_id cookie: " << l_cookies.text_id << "; current_pageno cookie: " << l_cookies.current_pageno << "; subtitle_id cookie: " << l_cookies.subtitle_id  << "\n";
        }

        std::string content = "<h1>404 Not Found</h1>"; 

        std::string content_type = "text/html;charset=UTF-8";
        
        char fil_ext[5];
        for(int i = 0; i < 4; i++) {
            fil_ext[i] = url_c_str[url_size - 5 + i];
        }
        fil_ext[4] = '\0';
        std::cout << "fil_ext = " << fil_ext << "\n";

        if(!strcmp(fil_ext, ".css")) {
            content_type = "text/css";
            sendBinaryFile(url_c_str, clientSocket, content_type);
            return;
        }
        else if(!strcmp(fil_ext + 1, ".js")) {
            content_type = "application/javascript";
            sendBinaryFile(url_c_str, clientSocket, content_type);
            return;
        }
        else if(!strcmp(fil_ext, ".ttf")) {
            content_type = "font/ttf";
            sendBinaryFile(url_c_str, clientSocket, content_type);
            return;
        }
        else if(!strcmp(fil_ext, ".mp3")) {
            content_type = "audio/mpeg";
            sendBinaryFile(url_c_str, clientSocket, content_type);
            return;
        }
        else if(!strcmp(fil_ext, ".png")) {
            content_type = "image/png";
            sendBinaryFile(url_c_str, clientSocket, content_type);
            return;
        }
        else if(!strcmp(fil_ext, ".svg")) {
            content_type = "image/svg+xml";
            sendBinaryFile(url_c_str, clientSocket, content_type);
            return;
        }
        else if(!strcmp(fil_ext, ".jpg")) {
            content_type = "image/jpeg";
            sendBinaryFile(url_c_str, clientSocket, content_type);
            return;
        } 


        buildGETContent(page_type, url_c_str, content, cookies_present, l_cookies);       

     

        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n";
    //    oss << "Cache-Control: no-cache, private \r\n";
        oss << "Content-Type: " << content_type << "\r\n";
        oss << "Content-Length: " << content.size() << "\r\n";
        if(page_type > 0)setCookies(oss, l_cookies);
        oss << "\r\n";
        oss << content;

        std::string output = oss.str();
        int size = output.size() + 1;

        sendToClient(clientSocket, output.c_str(), size);
    }
    else if(get_or_post == POST) {  

        std::cout << "This is a POST request" << std::endl;
        
       buildPOSTedData(msg, true, length);
       if(m_POST_continue == false) {
            handlePOSTedData(m_post_data, clientSocket);
       } 
    } 
    
}


// bool OcsServer::c_strStartsWith(const char *str1, const char *str2)
// {
//     int strcmp_count = 0;
//     int str2_len = strlen(str2);
//
//     int i = -1;
//
//     while ((*str1 == *str2 || *str2 == '\0') && i < str2_len)
//     {
//         strcmp_count++;
//         str1++;
//         str2++;
//         i++;
//     }
//
//     if (strcmp_count == str2_len + 1)
//     {
//         return true;
//     }
//     else
//         return false;
// }
bool OcsServer::c_strStartsWith(const char *haystack, const char *needle) {
    while(*needle) {
        if(*haystack != *needle) {
            return false;
        }
        ++haystack;
        ++needle;
    }
    return true;
}

int OcsServer::c_strFind(const char* haystack, const char* needle) {

    int needle_startpos = 0;
    int needle_length = strlen(needle);
    int haystack_length = strlen(haystack);
    if(haystack_length < needle_length) return -1;
    char needle_buf[needle_length + 1]; //yes I'm stack-allocating variable-length arrays because g++ lets me and I want the efficiency; it will segfault just the same if I pre-allocate an arbitrary length array which the data is too big for anyway, and I absolutely will not use any of the heap-allocated C++ containers for the essential HTTP message parsing, which needs to be imperceptibly fast

    needle_buf[needle_length] = '\0';
    for(int i = 0; i < haystack_length; i++) {
        for(int j = 0 ; j < needle_length; j++) {
            needle_buf[j] = haystack[j];
        }
       
        if(c_strStartsWith(needle_buf, needle)) {
            break;
        }
        needle_startpos++;
        haystack++;
    }
    
    if(needle_startpos == haystack_length) {
        return -1;
        }
    else {
        return needle_startpos;
    } 
}

int OcsServer::c_strFindNearest(const char* haystack, const char* needle1, const char* needle2) {
    int needles_startpos = 0;
    int needle1_length = strlen(needle1);
    int needle2_length = strlen(needle2);
    int longest_needle_length = needle1_length < needle2_length ? needle2_length : needle1_length;
    int haystack_length = strlen(haystack);
    if(haystack_length < longest_needle_length) return -1;

    char needles_buf[longest_needle_length + 1];
    needles_buf[longest_needle_length] = '\0';

    for(int i = 0; i < haystack_length; i++) {
        for(int j = 0; j < longest_needle_length; j++) {
            needles_buf[j] = haystack[j];
        }
        if(c_strStartsWith(needles_buf, needle1) || c_strStartsWith(needles_buf, needle2)) {
            break;
        }
        needles_startpos++;
        haystack++;
    }

    if(needles_startpos == haystack_length) {
        return -1;
    }
    else {
        return needles_startpos;
    }
}


int OcsServer::getRequestType(const char* msg ) {

    if(c_strStartsWith(msg, "GET")) {
        return 3;
    }
    else if(c_strStartsWith(msg, "POST")) {
        return 4;
    }
    else return -1;
}

int OcsServer::checkHeaderEnd(const char* msg) {
    int lb_pos = c_strFind(msg, "\x0d");
    int HTTP_line_pos = c_strFind(msg, "HTTP/1.1");
    if(HTTP_line_pos != -1 && HTTP_line_pos < lb_pos) return lb_pos;
    else return -1;
}

 void OcsServer::buildGETContent(short int page_type, char* url_c_str, std::string &content, bool cookies_present, Cookies& l_cookies) {
    
    std::ifstream urlFile;
    urlFile.open(url_c_str);
    
    if (urlFile.good())
    {
        std::cout << "This file exists and was opened successfully." << std::endl;

        std::ostringstream ss_text;
        std::string line;

        std::string l_page_toknos_arr;

        while (std::getline(urlFile, line))
        {   
            if(page_type > 0 && line.find("<?php") != -1) insertTextSelect(ss_text, l_cookies);
            else if(page_type == 1 && line.find("<?cky") != -1) void_retrieveText(ss_text, l_page_toknos_arr, l_cookies); 
            //else if(page_type == 1 && line.find("<?cky") != -1) ss_text << "<br><br><div id=\"textbody\"></div>\n";
            
            //For some reason I do not understand, if you insert a <script> tag to declare the below JS variable outside of the script tag at the bottom, on Chrome Android the server very often will get stuck for absolutely ages on loading the Bookerly font file when you refresh the page, and the javascript engine will freeze for about 5-10seconds, but this never happens on Desktop. Thus I've had to make up another bullshit <?js tag to signal where to insert this C++-generated JS
            else if(page_type == 1 /*&& cookies_present*/ && line.find("<?js") != -1) { //the insertTextSelect() function should set all the cookie-values if they are absent, and I need this to run when there are no cookies inorder to set the page_toknos_arr javascript-array to the correct value or functions will break
                ss_text << "let cookie_textselect = " << /* m_cookies[0] */ l_cookies.text_id  << ";\n";
                ss_text << "page_toknos_arr = " << l_page_toknos_arr << ";\n";
                ss_text << "if(page_toknos_arr.length > 1) {document.getElementById('pagenos').addEventListener('click', selectText_splitup);\ndocument.getElementById('pagenos').addEventListener('keydown', selectText_splitup);}\n";
                //ss_text << "dt_end = " << m_dt_end << ";\n";
                ss_text << "current_pageno = " << /* m_cookies[1] */ l_cookies.current_pageno << ";\n";
            }
            else if(page_type == 1 && line.find("<?js") != -1) ss_text << "let cookie_textselect = 0;\n";

            else ss_text << line << '\n';
        }

        content = ss_text.str();

        urlFile.close();
    }
    else
    {
        std::cout << "This file was not opened successfully." << std::endl;
    }
        
}


void OcsServer::insertTextSelect(std::ostringstream &html, Cookies& l_cookies) {
    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open(m_DB_path, &DB)) {
        int prep_code, run_code;
        const char *sql_text;

        sql_text = "SELECT text_id, text_title, tokno_start, tokno_end FROM texts";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);

        int text_id = 0;
        // int cookie_text_id = safeStrToInt(m_cookies[0]);
        int cookie_text_id = safeStrToInt(l_cookies.text_id);
        bool cookie_text_id_exists = false;
        // int cookie_subtitle_id = safeStrToInt(m_cookies[2]);
        int cookie_subtitle_id = safeStrToInt(l_cookies.subtitle_id);
        const char* text_title;
        int tokno_start = 0;
        int tokno_end = 0;
        //std::string text_title_str;

        while(sqlite3_step(statement) == SQLITE_ROW) {
            text_id = sqlite3_column_int(statement, 0);
            text_title = (const char*)sqlite3_column_text(statement, 1);
            tokno_start = sqlite3_column_int(statement, 2);
            tokno_end = sqlite3_column_int(statement, 3);

            html << "<option value=\"" << text_id << "\" data-tokno_start=\"" << tokno_start << "\" data-tokno_end=\"" << tokno_end <<  "\"";
            if(cookie_text_id == text_id) {
                html << "selected";
                cookie_text_id_exists = true;
            }
            html << ">" << text_title << "</option>\n";
            //text_title_str = "";
        }
        sqlite3_finalize(statement);
        if(!cookie_text_id_exists) {
            //m_cookies[0] = "1";
            l_cookies.text_id = "1";
            // m_cookies[1] = "1";
            l_cookies.current_pageno = "1";
            cookie_text_id = 1;
        }
        else {
            // m_cookies[0] = std::to_string(cookie_text_id);
            l_cookies.text_id = std::to_string(cookie_text_id);
        }
        html << "</select><select id=\"subtitle_select\" name=\"chapselect\" autocomplete=\"off\" style=\"width: 150px;\">";

        sql_text = "SELECT subtitle_id, subtitle_text, tokno_start, tokno_end FROM subtitles WHERE text_id = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, cookie_text_id);
        int subtitle_id = 0;
        bool cookie_subtitle_id_exists = false;
        int first_subtitle_id = 0;
        const char* subtitle_text;
        tokno_start = 0;
        tokno_end = 0;

        sqlite3_step(statement);
        subtitle_id = sqlite3_column_int(statement, 0);
        first_subtitle_id = subtitle_id;
        subtitle_text = (const char*)sqlite3_column_text(statement, 1);
        tokno_start = sqlite3_column_int(statement, 2);
        tokno_end = sqlite3_column_int(statement, 3);

        html << "<option value=\"" << subtitle_id << "\" data-tokno_start=\"" << tokno_start << "\" data-tokno_end=\"" << tokno_end <<  "\"";
        if(cookie_subtitle_id == subtitle_id) {
            html << "selected";
            cookie_subtitle_id_exists = true;
        }
        html << ">" << subtitle_text << "</option>\n";

        while(sqlite3_step(statement) == SQLITE_ROW) {
            subtitle_id = sqlite3_column_int(statement, 0);
            subtitle_text = (const char*)sqlite3_column_text(statement, 1);
            tokno_start = sqlite3_column_int(statement, 2);
            tokno_end = sqlite3_column_int(statement, 3);

            html << "<option value=\"" << subtitle_id << "\" data-tokno_start=\"" << tokno_start << "\" data-tokno_end=\"" << tokno_end <<  "\"";
            if(cookie_subtitle_id == subtitle_id) {
                html << "selected";
                cookie_subtitle_id_exists = true;
            }
            html << ">" << subtitle_text << "</option>\n";
        }

        sqlite3_finalize(statement);
        if(!cookie_subtitle_id_exists) {
            // m_cookies[2] = std::to_string(first_subtitle_id);
            l_cookies.subtitle_id = std::to_string(first_subtitle_id);
            // m_cookies[1] = "1";
            l_cookies.current_pageno = "1";
        }
        else {
            // m_cookies[2] = std::to_string(cookie_subtitle_id);
            l_cookies.subtitle_id = std::to_string(cookie_subtitle_id);
        }
        html << "</select>";
        sqlite3_close(DB); 
    }
}

void OcsServer::buildPOSTedData(const char* msg, bool headers_present, int length) {

    if(headers_present) {

        setURL(msg);
        
        int content_length_start = c_strFind(msg, "Content-Length:") + 16;
        
        int cl_figures = 0;
        char next_nl = 'c';
        while(next_nl != '\x0d') {
            next_nl = msg[content_length_start + cl_figures];
            cl_figures++;
        }
        cl_figures--;
        std::cout << "Number of digits in content-length: " << cl_figures << std::endl;

        char content_length_str[cl_figures + 1];
        for(int i = 0; i < cl_figures; i++) {
            content_length_str[i] = msg[content_length_start + i];
        }
        content_length_str[cl_figures] = '\0';
        int content_length = atoi(content_length_str);
        std::cout << "Parsed-out content-length: " << content_length << std::endl;

        int headers_length = c_strFind(msg, "\r\n\r\n");
        m_POST_continue = false;
        m_bytes_handled = 0;
        m_total_post_bytes = headers_length + 4 + content_length;
        if(m_total_post_bytes > length) {
            std::string post_data {msg + headers_length + 4}; //strcat won't work unless I use malloc() to allocate heap-memory of appropriate size for the stuck-together c-strings, so may as well use std::string if I'm forced to heap-allocate anyway
            //at least I'm only heap-allocating in those instances where the POST data makes the message over 4096 bytes
           m_post_data_incomplete = post_data;
           // std::cout << "headers_length + 4 + content_length > length" << std::endl;
           m_POST_continue = true;
           m_bytes_handled += length;
            
        }
        else {
            m_post_data = msg + headers_length + 4;
        }
  
    }
    else {
        
        std::string post_data_cont {msg};
        m_post_data_incomplete.append(post_data_cont);
        m_post_data = m_post_data_incomplete.c_str();
        m_bytes_handled += length;
        if(m_total_post_bytes == m_bytes_handled) {
            m_POST_continue = false;
        }    
    }

}

std::string OcsServer::URIDecode(std::string &text) //stolen off a rando on stackexchange who forgot to null-terminate the char-array before he strtol()'d it, which caused big problems
{
    std::string escaped;

    for (auto i = text.begin(), nd = text.end(); i < nd; ++i)
    {
        auto c = (*i);

        switch (c)
        {
        case '%':
            if (i[1] && i[2])
            {
                char hs[]{i[1], i[2], '\0'};
                escaped += static_cast<char>(strtol(hs, nullptr, 16));
                i += 2;
            }
            break;
        case '+':
            escaped += ' ';
            break;
        default:
            escaped += c;
        }
    }

    return escaped;
}

std::string OcsServer::htmlspecialchars(const std::string &innerHTML) {
    std::string escaped;

    for(auto i = innerHTML.begin(), nd = innerHTML.end(); i < nd; i++) {
        char c = (*i);

        switch(c) {
            case '<':
                escaped += "&lt;";
                break;
            case '>':
                escaped += "&gt;";
                break;
            case '"':
                escaped += "&quot;";
                break;
            case '\'':
                escaped += "&#039;";
                break;
            case '&':
                escaped += "&amp;";
                break;
            case '\x5c':
                escaped += "\x5c\x5c";
                break;
            case '\n':
                escaped += "\x5c\x6e"; //we need a literal 'n' for the JSON otherwise it is just a slash plus a newline character
                break;
            case '\t':
                escaped += "\x5c\x74";
                break;
            case '\r':
                escaped += "\x5c\x72";
                break;
            case '\f':
                escaped += "\x5c\x66";
                break;
            default:
                escaped += c;
        }
    }
    return escaped;
}

//I think the use-case for htmlspecialchars() over escapeQuotes() is when one is going to be building HTML-strings directly out of the response-data; in this case the ampersands and angle-brackets need to be escaped or they'll fuck up the HTML. However if one is changing the value of already-existing HTML elements via Javascript by setting them equal to the returned data, then we don't need to escape special HTML characters and only need to worry about JSON-escaping
std::string OcsServer::escapeQuotes(const std::string &quoteystring) {
    std::string escaped;
    for(auto i = quoteystring.begin(), nd = quoteystring.end(); i < nd; i++) {

        char c = (*i); //the quotation mark is always a single-byte ASCII char so this is fine
        switch(c) {
            case '\x22':
                escaped += "\\\"";
                break;
            case '\x5c':
                escaped += "\x5c\x5c";
                break;
            case '\n':
                escaped += "\x5c\x6e";
                break;
            case '\r':
                escaped += "\x5c\x72";
                break;
            case '\t':
                escaped += "\x5c\x74";
                break;
            case '\f':
                escaped += "\x5c\x66";
                break;
            default:
                escaped += c;
        }
    }
    return escaped;
}

std::string OcsServer::applySuperscripts(const std::string &database_text) {
    std::string superscript_text;

    for(auto i = database_text.begin(), nd = database_text.end(); i < nd; i++) {
        char c = (*i);
        switch(c) {
            case '$':
                superscript_text += "<sup>";
                break;
            case '@':
                superscript_text += "</sup>";
                break;
            default:
                superscript_text += c;
        }
    }
    return superscript_text;
}

//in order to avoid using std::vector I cannot save the array of the POSTed data into a variable which persists outside this function, because the size of the array is only known if I know which POST data I am processing, which means the code to create an array out of the m_post_data has to be repeated everytime a function to handle it is called
void OcsServer::handlePOSTedData(const char* post_data, int clientSocket) {
    std::cout << "------------------------COMPLETE POST DATA------------------------\n" << post_data << "\n-------------------------------------------------------\n";
    
    int post_fields = getPostFields(m_url);

    if(post_fields == -1) {
        std::string bad_post_response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 30\r\n\r\nUnrecognised POST url - repent";
        sendToClient(clientSocket, bad_post_response.c_str(), bad_post_response.length() + 1);
        return;
    }

    int equals_positions_arr[post_fields];
    int equals_pos = 0;
    for(int i = 0; i < post_fields; i++) {
        equals_pos = c_strFind(post_data + equals_pos + 1, "=") + equals_pos + 1;
        if(equals_pos == -1) break;
        equals_positions_arr[i] = equals_pos;
        
    }

    int amp_positions_arr[post_fields];
    int amp_pos = 0;
   
  
    for(int i = 0; i < post_fields - 1; i++) {
        amp_pos = c_strFind(post_data + amp_pos + 1, "&") + amp_pos + 1;
        if (amp_pos == -1) break;
        amp_positions_arr[i] = amp_pos;
        
    }
    amp_positions_arr[post_fields -1] = strlen(post_data);
    

    for(int i = 0; i < post_fields; i++) {
        std::cout << "equals position no. " << i << ": " << equals_positions_arr[i] << std::endl;
    }
    for(int i = 0; i < post_fields - 1; i++) {
        std::cout << "amp position no. " << i << ": " << amp_positions_arr[i] << std::endl;
    }
    std::string post_values[post_fields];
    for(int i = 0; i < post_fields; i++) {
        //this could be a really long text so if over a certain size needs to be heap-allocated (however it's also used for literally all POST requests, most of which are tiny, so the limit for stack-allocation will be 512Kb)
        int ith_post_value_length = amp_positions_arr[i] - equals_positions_arr[i] + 1;
        
        if(ith_post_value_length > 524288) {
            char* ith_post_value = new char[ith_post_value_length];
            std::cout << "length of post_value array: " << ith_post_value_length << std::endl;
            for(int j = 0; j < amp_positions_arr[i] - equals_positions_arr[i] - 1; j++) {
                ith_post_value[j] = post_data[equals_positions_arr[i] + 1 + j];
            // printf("j = %i, char = %c | ", j, post_data[equals_positions_arr[i] + 1 + j]);
            }
            ith_post_value[amp_positions_arr[i] - equals_positions_arr[i] - 1] = '\0';
            
            std::string ith_post_value_str {ith_post_value};
            delete[] ith_post_value;
      
            post_values[i] = ith_post_value_str;
        }
        else {
            char ith_post_value[ith_post_value_length];  //In Windows I will make this probably a 4096 bytes fixed-sized-array, because no-one needs 512Kb to hold a lang_id etc.
            std::cout << "length of post_value array: " << ith_post_value_length << std::endl;
            for(int j = 0; j < amp_positions_arr[i] - equals_positions_arr[i] - 1; j++) {
                ith_post_value[j] = post_data[equals_positions_arr[i] + 1 + j];
            // printf("j = %i, char = %c | ", j, post_data[equals_positions_arr[i] + 1 + j]);
            }
            ith_post_value[amp_positions_arr[i] - equals_positions_arr[i] - 1] = '\0';
            
            std::string ith_post_value_str {ith_post_value};
      
            post_values[i] = ith_post_value_str;
        }    
    }

    for (int i = 0; i < post_fields; i++)
    {
       std::cout << "POST value no." << i << ": " << post_values[i] << std::endl;
    }

    if(!strcmp(m_url, "/retrieve_subtitle.php")) {
        bool php_func_success = retrieveTextSubtitle(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/lemma_tooltip.php")) {
        bool php_func_success = lemmaTooltips(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/lcs_search.php")) {
        bool php_func_success = lcsSearch(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/lcs_trigram_search.php")) {
        bool php_func_success = lcsTrigramSearch(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/retrieve_text.php")) {
        bool php_func_success = retrieveText(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/retrieve_text_pageno.php")) {
        bool php_func_success = retrieveTextPageNo(post_values, clientSocket);
    }
    else if (!strcmp(m_url, "/retrieve_text_search.php")) {
        bool php_func_success = retrieveTextFromSearch(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/curl_lookup.php")) {
        bool php_func_success = curlLookup(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/gorazd_lookup.php")) {
        bool php_func_success = gorazdLookup(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/lcs_regex_search.php")) {
        bool php_func_success = lcsRegexSearch(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/greek_tooltips.php")) {
        bool php_func_success = greekTooltips(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/generate_inflection.php")) {
        bool php_func_success = generateInflection(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/cyr_search.php")) {
        bool php_func_success = cyrSearch(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/cyr_fuzzy_search.php")) {
        bool php_func_success = cyrFuzzySearch(post_values, clientSocket);
    }

    std::cout << "m_url: " << m_url << std::endl;
    
}

void OcsServer::setURL(const char* msg) {
    int url_start = c_strFind(msg, "/");
  //  printf("url_start: %i\n", url_start);
    int url_end = c_strFind(msg + url_start, " ") + url_start;
  //  printf("url_end: %i\n", url_end);
    //char url[url_end - url_start + 1];
    char url[50];
    memset(url, '\0', 50);
    for (int i = 0; i < url_end - url_start && i < 49; i++)
    {
        url[i] = msg[url_start + i];
    }
    //url[url_end - url_start] = '\0';
    
    strcpy(m_url, url); //m_url is max 50 chars but this is allowed because I tightly control what the POST urls are; using std::string would be wasteful
}

int OcsServer::getPostFields(const char* url) {
    if(!strcmp(url, "/update_db.php")) return 3;
    else if(!strcmp(url, "/retrieve_subtitle.php")) return 2;
    else if(!strcmp(url, "/lemma_tooltip.php")) return 1;
    else if(!strcmp(url, "/lcs_search.php")) return 3;
    else if(!strcmp(url, "/lcs_trigram_search.php")) return 3;
    else if(!strcmp(url, "/retrieve_text.php")) return 1;
    else if(!strcmp(url, "/retrieve_text_pageno.php")) return 2;
    else if (!strcmp(url, "/retrieve_text_search.php")) return 1;
    else if(!strcmp(url, "/curl_lookup.php")) return 1;
    else if(!strcmp(url, "/gorazd_lookup.php")) return 2;
    else if(!strcmp(url, "/lcs_regex_search.php")) return 3;
    else if(!strcmp(url, "/greek_tooltips.php")) return 2;
    else if(!strcmp(url, "/generate_inflection.php")) return 3;
    else if(!strcmp(url, "/cyr_search.php")) return 3;
    else if(!strcmp(url, "/cyr_fuzzy_search.php")) return 3;
    else return -1;
}

void OcsServer::setCookies(std::ostringstream &http_response_ss, Cookies& l_cookies) {
  auto cookie_values = l_cookies.asArray();
   for(size_t i = 0; i < std::size(cookie_values); ++i) {
    http_response_ss << "Set-Cookie:" << l_cookies.keys[i] << *cookie_values[i] << "; Max-Age=157680000\r\n";
    std::cout << "Set-Cookie:" << l_cookies.keys[i] << *cookie_values[i] << "; Max-Age=157680000\r\n";
   } 
} 

bool OcsServer::readCookie(const char* msg, Cookies& l_cookies) {
    int cookie_start = c_strFind(msg, "\r\nCookie") + 9;
    if(cookie_start == 8) {
        
        l_cookies.text_id = "4";
        l_cookies.current_pageno = "1";
        l_cookies.subtitle_id = "1";
        return false; //c_strFind() returns -1 if unsuccessful, but I've just added 9 to it so the number signalling no cookies is 8
    }

    std::array<std::string*, 3> cookies_values = l_cookies.asArray();
    for(size_t i = 0; i < std::size(l_cookies.keys); ++i) {
        int cookieName_start = c_strFind(msg+cookie_start, l_cookies.keys[i]);
        if(i == 0 && cookieName_start == -1) {
            *cookies_values[i] = "4";
            return false;
        }
        if(cookieName_start == -1) {
            *cookies_values[i] = "1";
            break;
        }

        int cookieName_length = strlen(l_cookies.keys[i]);

        int val_length = c_strFindNearest(msg+cookie_start+cookieName_start + cookieName_length, ";", "\r\n");

        char val[val_length + 1];
        for(int j = 0; j < val_length; j++) {
            val[j] = (msg + cookie_start + cookieName_start + cookieName_length)[j];
        }
        val[val_length] = '\0';

        *cookies_values[i] = val;
    }

    return true;
}

int OcsServer::safeStrToInt(const std::string &string_number, int default_result) {
    int converted_int = default_result;
    try {
        converted_int = std::stoi(string_number);
    }
    catch (std::invalid_argument const& ex) {
        std::cout << "std::stoi failed with an invalid_argument exception; defaulting it to " << default_result << "\n";
    }
    catch (std::out_of_range const& ex) {
        std::cout << "std::stoi failed with an out_of_range exception; defaulting it to " << default_result << "\n";
    }
    return converted_int;
}
sqlite3_int64 OcsServer::safeStrToInt64(const std::string &string_number, sqlite3_int64 default_result) {
    sqlite3_int64 converted_int = default_result;
    try {
        converted_int = std::stol(string_number);
    }
    catch (std::invalid_argument const& ex) {
        std::cout << "std::stoi failed with an invalid_argument exception; defaulting it to " << default_result << "\n";
    }
    catch (std::out_of_range const& ex) {
        std::cout << "std::stoi failed with an out_of_range exception; defaulting it to " << default_result << "\n";
    }
    return converted_int;
}

bool OcsServer::lemmaTooltips(std::string _POST[1], int clientSocket) {
    sqlite3* DB;    
    
    if(!sqlite3_open(m_DB_path, &DB)) {

        sqlite3_stmt* statement1;     

        int lemma_ids_count = 1;
        for(auto i = _POST[0].begin(), nd=_POST[0].end(); i < nd; i++) {
            char c = (*i);
            if(c == ',') lemma_ids_count++;
        }

        

        std::vector<int> lemma_ids;
        lemma_ids.reserve(lemma_ids_count); 

        std::string lemma_id_str = "";
        for(auto i = _POST[0].begin(), nd=_POST[0].end(); i < nd; i++) {
            char c = (*i);
            if(c == ',') {
                int lemma_id = safeStrToInt(lemma_id_str);
                lemma_ids.emplace_back(lemma_id);
                lemma_id_str = "";
                continue;
            }
            lemma_id_str += c;
            if(nd - 1 == i) {
                int lemma_id = safeStrToInt(lemma_id_str);
                lemma_ids.emplace_back(lemma_id);
            }
        }
        

        const char* sql_text1 = "SELECT pos, lemma_ocs FROM lemmas WHERE lemma_id = ?";
        sqlite3_prepare_v2(DB, sql_text1, -1, &statement1, NULL);

        std::string lemma_ocs = "";
        short int pos = 1;
        //const char* lemma_ocs;

        std::ostringstream pos_array;
        pos_array << "[";
        std::ostringstream lemma_ocs_array;
        lemma_ocs_array << "[";
        
   
        for(const int &lemma_id : lemma_ids) {
            sqlite3_bind_int(statement1, 1, lemma_id);
            sqlite3_step(statement1);
            pos = sqlite3_column_int(statement1, 0);
            lemma_ocs = (const char*)sqlite3_column_text(statement1, 1);

            //std::cout << "pos: " << pos << "; lemma_ocs: " << lemma_ocs << "\n";

            lemma_ocs_array << "\"" << htmlspecialchars(lemma_ocs) << "\"" << ',';
            pos_array << pos << ',';

            sqlite3_reset(statement1);
            sqlite3_clear_bindings(statement1);
        }
        sqlite3_finalize(statement1);
        
        lemma_ocs_array.seekp(-1, std::ios_base::cur);
        pos_array.seekp(-1, std::ios_base::cur);       

        pos_array << "]";
        lemma_ocs_array << "]";

        std::string json_str = "[" + pos_array.str() + "," + lemma_ocs_array.str() + "]";
        int content_length = json_str.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
        sqlite3_close(DB);
       
        return true;
    }
    else {
        std::cout << "Database connection failed in lemmaTooltips()" << std::endl;
        return false;
    }
}

bool OcsServer::lcsSearch(std::string _POST[3], int clientSocket) {
    sqlite3* DB;    
    
    if(!sqlite3_open(m_DB_path, &DB)) {

        sqlite3_stmt* statement1;
        sqlite3_stmt* statement2;     
        std::string lcs_search_query = "%"+URIDecode(_POST[0])+"%";
        int tokno_start = safeStrToInt(_POST[1]);
        int tokno_end = safeStrToInt(_POST[2]);

        const char* sql_text1 = "SELECT tokno, autoreconstructed_lcs, sentence_no FROM corpus WHERE tokno >= ? AND tokno <= ? AND autoreconstructed_lcs LIKE ?";
        const char* sql_text2 = "SELECT tokno, chu_word_torot, presentation_before, presentation_after FROM corpus WHERE tokno < ? AND tokno > ? AND sentence_no = ?";
        sqlite3_prepare_v2(DB, sql_text1, -1, &statement1, NULL);
        sqlite3_prepare_v2(DB, sql_text2, -1, &statement2, NULL);
         
        sqlite3_bind_int(statement1, 1, tokno_start);
        sqlite3_bind_int(statement1, 2, tokno_end);
        sqlite3_bind_text(statement1, 3, lcs_search_query.c_str(), -1, SQLITE_STATIC);

        std::ostringstream lcs_results;
        lcs_results << "[";
        std::ostringstream text_results;
        text_results << "[";
        std::ostringstream tokno_results;
        tokno_results << "[";

        int tokno = 0;
        std::string lcs_result = "";
        std::string chu_word_torot = "";
        std::string present_before = "";
        std::string presentation_after = "";

        int sentence_no = 0;
        int results_count = 0;
   
        while(sqlite3_step(statement1) == SQLITE_ROW) {
            tokno = sqlite3_column_int(statement1, 0);
            lcs_result = (const char*)sqlite3_column_text(statement1, 1);
            sentence_no = sqlite3_column_int(statement1, 2);
            //std::cout << "lcs_result: " << lcs_result << "\n";
            lcs_results << "\"" << lcs_result << "\"" << ",";
            tokno_results << tokno << ",";

            std::ostringstream text_content_result_oss;

            sqlite3_bind_int(statement2, 1, tokno + 5);
            sqlite3_bind_int(statement2, 2, tokno - 5);
            sqlite3_bind_int(statement2, 3, sentence_no);
            while(sqlite3_step(statement2) == SQLITE_ROW) {
                text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 2));
                if(sqlite3_column_int(statement2, 0) == tokno) {
                    text_content_result_oss << "<span class=\"result_word\">" + applySuperscripts((const char*)sqlite3_column_text(statement2, 1)) + "</span>";
                }
                else text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 1));
                
                text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 3));
            }
            text_results << "\"" << escapeQuotes(text_content_result_oss.str()) << "\",";


            sqlite3_reset(statement2);
            sqlite3_clear_bindings(statement2);
            results_count++;

            if(results_count > 5000) {

                std::cout << "too many results\n";
                std::string json_str = "{\"error\":\"Too many results, please narrow the search query\"}";
                int content_length = json_str.size();

                std::ostringstream post_response;
                post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

                int length = post_response.str().size() + 1;
                sendToClient(clientSocket, post_response.str().c_str(), length);

                sqlite3_finalize(statement1);
                sqlite3_finalize(statement2);

                sqlite3_close(DB);
                return true;
            }
        }
        sqlite3_finalize(statement1);
        sqlite3_finalize(statement2);
                
        if(results_count > 0) {
            lcs_results.seekp(-1, std::ios_base::cur);
            text_results.seekp(-1, std::ios_base::cur);
            tokno_results.seekp(-1, std::ios_base::cur);
        }

        lcs_results << "]";
        text_results << "]";
        tokno_results << "]";    

        std::string json_str = "[" + lcs_results.str() + "," + text_results.str() + "," + tokno_results.str() + "]";
        int content_length = json_str.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
        sqlite3_close(DB);
       
        return true;
    }
    else {
        std::cout << "Database connection failed in lcsSearch()" << std::endl;
        return false;
    }
}

void OcsServer::writeTextBody(std::ostringstream &html, sqlite3* DB, int tokno_start, int tokno_end) {
    const char* sql_text = "SELECT chu_word_torot, presentation_before, presentation_after, sentence_no, lemma_id, morph_tag, autoreconstructed_lcs, inflexion_class, tokno, auto_tagged, pv2_3_exists FROM corpus WHERE tokno >= ? AND tokno <= ?";
    sqlite3_stmt* statement;
    sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
    sqlite3_bind_int(statement, 1, tokno_start);
    sqlite3_bind_int(statement, 2, tokno_end);
    const char* chu_word_torot;
    const char* presentation_before;
    const char* presentation_after;
    const char* morph_tag;
    std::string autoreconstructed_lcs;
    int lemma_id = 0;
    int sentence_number_previous = 0;
    int sentence_no_current = 0;
    std::string inflexion_class;
    int tokno = 0;
    int auto_tagged = 0;
    int pv2_3_exists = 0;
    while(sqlite3_step(statement) == SQLITE_ROW) {
        chu_word_torot = (const char*)sqlite3_column_text(statement, 0);
        presentation_before = (const char*)sqlite3_column_text(statement, 1);
        presentation_after = (const char*)sqlite3_column_text(statement, 2);
        sentence_no_current = sqlite3_column_int(statement, 3);
        lemma_id = sqlite3_column_int(statement, 4);
        const unsigned char* raw_sqlite_text = sqlite3_column_text(statement, 7);
        if(raw_sqlite_text != nullptr) {
            inflexion_class = (const char*)raw_sqlite_text;
        }
        tokno = sqlite3_column_int(statement, 8);
        auto_tagged = sqlite3_column_int(statement, 9);
        pv2_3_exists = sqlite3_column_int(statement, 10);
        if(sqlite3_column_type(statement, 5) == SQLITE_NULL) morph_tag = "";
        else morph_tag = (const char*)sqlite3_column_text(statement, 5);
        if(sqlite3_column_type(statement, 6) == SQLITE_NULL) autoreconstructed_lcs = "";
        else autoreconstructed_lcs = (const char*)sqlite3_column_text(statement, 6);

        if(sentence_number_previous > 0 && sentence_no_current != sentence_number_previous) {
            html << "  |  ";
        }

        html << "<span class=\"chunk\">" << applySuperscripts(presentation_before) << "<span class=\"tooltip\" data-tokno=\"" << tokno << "\" data-sentence_no=\"" << sentence_no_current << "\" data-auto_tagged=\"" << auto_tagged << "\" data-pv3=\"" << pv2_3_exists << "\""; 
        if(lemma_id > 0) {
            html << " data-lemma_id=\"" << lemma_id << "\"" " data-morph_tag=\"" << morph_tag << "\"";
        }
        if(!autoreconstructed_lcs.empty()) {
            html << " data-lcs_recon=\"" << autoreconstructed_lcs << "\" data-inflexion=\"" << inflexion_class << "\"";
        }
        html << ">" << applySuperscripts(chu_word_torot) << "</span>" << applySuperscripts(presentation_after) << "</span>";
        sentence_number_previous = sentence_no_current;
        autoreconstructed_lcs.clear();

    };
    sqlite3_finalize(statement);
}

bool OcsServer::retrieveText(std::string _POST[1], int clientSocket) {

    int sentences_per_page = m_sentences_per_page;
    std::ostringstream page_toknos_arr;
    page_toknos_arr << "[";
    int pageno_count = 1;
    int sentence_count = 0;

    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open(m_DB_path, &DB)) {

        int text_id = safeStrToInt(_POST[0]);

        const char *sql_text;

        sql_text = "SELECT subtitle_id, tokno_start, tokno_end, subtitle_text FROM subtitles WHERE text_id = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, text_id);
        sqlite3_step(statement);
        int first_subtitle_id = sqlite3_column_int(statement, 0);
        int first_tokno_start = sqlite3_column_int(statement, 1);
        int first_tokno_end = sqlite3_column_int(statement, 2);
        std::string subtitle_text = (const char*)sqlite3_column_text(statement, 3);

        std::ostringstream subtitle_toknos_json;
        subtitle_toknos_json<< "{\"" << first_subtitle_id << "\":[" << first_tokno_start << "," << first_tokno_end << ",\"" << escapeQuotes(subtitle_text) << "\"" << "],";
        while(sqlite3_step(statement) == SQLITE_ROW) {
            int subtitle_id = sqlite3_column_int(statement, 0);
            int subtitle_tokno_start = sqlite3_column_int(statement, 1);
            int subtitle_tokno_end = sqlite3_column_int(statement, 2);
            subtitle_text = (const char*)sqlite3_column_text(statement, 3);

            subtitle_toknos_json << "\"" << subtitle_id << "\":[" << subtitle_tokno_start << "," << subtitle_tokno_end << ",\"" << escapeQuotes(subtitle_text) << "\"" << "],";
        }
        subtitle_toknos_json.seekp(-1, std::ios_base::cur);
        subtitle_toknos_json << "}";

        sqlite3_finalize(statement);

        sql_text = "SELECT sentence_no, tokno FROM corpus WHERE rowid >= ? AND rowid <= ? GROUP BY sentence_no ORDER BY tokno";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, first_tokno_start);
        sqlite3_bind_int(statement, 2, first_tokno_end);

        int first_page_toknos[2] {first_tokno_start, 0};
        //I don't know whether the sentence_nos are guaranteed to be contiguous so I have to count each row rather than just do "last - first" to get the total number of sentences
        //if tokens have been deleted the start or end of a text then the page_toknos values won't actually correspond to the start and end-toknos, but since it's using >= and <= it should still work. Insertion of tokens into a text is not possible at all with this structure of the corpus table; the whole text would have to be re-inserted and the texts and subtitles tables updated
        page_toknos_arr << "[" << first_tokno_start;
        while(sqlite3_step(statement) == SQLITE_ROW) {
            int sentence_no = sqlite3_column_int(statement, 0);
            int sentence_start_tokno = sqlite3_column_int(statement, 1);
            // sentence_count++;

            if(sentence_count > 0 && sentence_count % sentences_per_page == 0) {
                page_toknos_arr << "," << sentence_start_tokno - 1 << "],[" << sentence_start_tokno;
                if(pageno_count == 1) first_page_toknos[1] = sentence_start_tokno;
                pageno_count++;
            }

            sentence_count++;
        }
        if(pageno_count == 1) first_page_toknos[1] = first_tokno_end + 1;
        page_toknos_arr << "," << first_tokno_end << "]]";
        //m_page_toknos_arr = page_toknos_arr.str();
        //m_pageno_count = pageno_count;
        sqlite3_finalize(statement);

        //m_cookies[1] = "1";

        std::ostringstream html;
        html << "<br><div id=\"textbody\">";
        writeTextBody(html, DB, first_page_toknos[0], first_page_toknos[1] - 1);

        // sql_text = "SELECT chu_word_torot, presentation_before, presentation_after, sentence_no FROM corpus WHERE tokno >= ? AND tokno <= ?"; // OR text_word = '\n')";
        // sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        // sqlite3_bind_int(statement, 1, first_page_toknos[0]);
        // sqlite3_bind_int(statement, 2, first_page_toknos[1] - 1);
        // const char* chu_word_torot;
        // const char* presentation_before;
        // const char* presentation_after;
        // int sentence_number_previous = 0;
        // int sentence_no_current = 0;

        
        // while(sqlite3_step(statement) == SQLITE_ROW) {
        //     chu_word_torot = (const char*)sqlite3_column_text(statement, 0);
        //     presentation_before = (const char*)sqlite3_column_text(statement, 1);
        //     presentation_after = (const char*)sqlite3_column_text(statement, 2);
        //     sentence_no_current = sqlite3_column_int(statement, 3);

        //     if(sentence_number_previous > 0 && sentence_no_current != sentence_number_previous) {
        //         html << "  |  ";
        //     }

        //     html << "<span class=\"chunk\">" << presentation_before << "<span class=\"tooltip\" data-sentence_no=\"" << sentence_no_current << "\">" << chu_word_torot << "</span>" << presentation_after << "</span>";
        //     sentence_number_previous = sentence_no_current;

        // };
        // sqlite3_finalize(statement);
        html << "</div>";       

        if(pageno_count > 1) {
            html << "<br><br><div id='pageno_footer'><div id=\"pagenos\">";
            html << "<div id=\"pageno_leftarrow\" class=\"nav_arrow\">&lt;</div>";
            html << "<textarea id=\"pageno_box\" spellcheck=\"false\" autocomplete=\"off\">";
            html << "1";
            html << "</textarea>";
            html << "<div class=\"pageno_text\" style=\"width: 40px;\">of</div>";
            html << "<div class=\"pageno_text\">" << pageno_count << "</div>";
            
            html << "<div id=\"pageno_rightarrow\" class=\"nav_arrow\">&gt;</div>";             
            html << "</div></div>";
        }
        else {
            html << "<br>";
        }
        //std::cout << m_page_toknos_arr << "\n";

        std::stringstream json_response_ss;
        json_response_ss << "{\"html\":\"" << escapeQuotes(html.str()) << "\",\"pagenos\":" << page_toknos_arr.str() << ",\"subtitles_json\":" << subtitle_toknos_json.str() << "}";

        int content_length = json_response_ss.str().size();
        std::ostringstream post_response_ss;
        post_response_ss << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n";
        post_response_ss << "Set-Cookie: text_id=" << text_id << "; Max-Age=157680000\r\n";
        post_response_ss << "Set-Cookie: subtitle_id=" << first_subtitle_id << "; Max-Age=157680000\r\n";
        post_response_ss << "Set-Cookie: current_pageno=1; Max-Age=157680000\r\n\r\n" << json_response_ss.str();
        int length = post_response_ss.str().size() + 1;
        sendToClient(clientSocket, post_response_ss.str().c_str(), length);

        sqlite3_close(DB);
        return true;

    }
    else {       
        
        std::cout << "Database connection failed on retrieveText()<br><br>\n";
        return false;
    }
}
bool OcsServer::retrieveTextSubtitle(std::string _POST[2], int clientSocket) {

    int sentences_per_page = m_sentences_per_page;
    std::ostringstream page_toknos_arr;
    page_toknos_arr << "[";
    int pageno_count = 1;
    float sentence_count = 0;

    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open(m_DB_path, &DB)) {

        int tokno_start = safeStrToInt(_POST[0]);
        int tokno_end = safeStrToInt(_POST[1]);

        const char *sql_text;

        sql_text = "SELECT sentence_no, tokno FROM corpus WHERE rowid >= ? AND rowid <= ? GROUP BY sentence_no ORDER BY tokno";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, tokno_start);
        sqlite3_bind_int(statement, 2, tokno_end);

        int first_page_toknos[2] {tokno_start, 0};
        //I don't know whether the sentence_nos are guaranteed to be contiguous so I have to count each row rather than just do "last - first" to get the total number of sentences
        //the way I am doing this means it will break if you delete (or insert, but that's impossible anyway) a token from within a text
        page_toknos_arr << "[" << tokno_start;
        while(sqlite3_step(statement) == SQLITE_ROW) {
            int sentence_no = sqlite3_column_int(statement, 0);
            int sentence_start_tokno = sqlite3_column_int(statement, 1);
            // sentence_count++;
            float quotient = trunc(sentence_count / (float)sentences_per_page);
            int remainder = (int)((sentence_count - (float)sentences_per_page*quotient));
            if(sentence_count > 0 && remainder == 0) {
                page_toknos_arr << "," << sentence_start_tokno - 1 << "],[" << sentence_start_tokno;
                if(pageno_count == 1) first_page_toknos[1] = sentence_start_tokno;
                pageno_count++;
            }
            sentence_count++;
        }
        if(pageno_count == 1) first_page_toknos[1] = tokno_end + 1;
        page_toknos_arr << "," << tokno_end << "]]";
        //m_page_toknos_arr = page_toknos_arr.str();
        //m_pageno_count = pageno_count;
        sqlite3_finalize(statement);

        std::ostringstream html;
        html << "<br><div id=\"textbody\">";
        writeTextBody(html, DB, first_page_toknos[0], first_page_toknos[1] - 1);
        html << "</div>";       

        if(pageno_count > 1) {
            html << "<br><br><div id='pageno_footer'><div id=\"pagenos\">";
            html << "<div id=\"pageno_leftarrow\" class=\"nav_arrow\">&lt;</div>";
            html << "<textarea id=\"pageno_box\" spellcheck=\"false\" autocomplete=\"off\">";
            html << "1";
            html << "</textarea>";
            html << "<div class=\"pageno_text\" style=\"width: 40px;\">of</div>";
            html << "<div class=\"pageno_text\">" << pageno_count << "</div>";
            
            html << "<div id=\"pageno_rightarrow\" class=\"nav_arrow\">&gt;</div>";             
            html << "</div></div>";
        }
        else {
            html << "<br>";
        }
        //std::cout << m_page_toknos_arr << "\n";

        std::stringstream json_response_ss;
        json_response_ss << "{\"html\":\"" << escapeQuotes(html.str()) << "\",\"pagenos\":" << page_toknos_arr.str() << "}";

        int content_length = json_response_ss.str().size();
        std::ostringstream post_response_ss;
        post_response_ss << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_response_ss.str();
        int length = post_response_ss.str().size() + 1;
        sendToClient(clientSocket, post_response_ss.str().c_str(), length);

        sqlite3_close(DB);
        return true;

    }
    else {       
        
        std::cout << "Database connection failed on retrieveTextSubtitle()<br><br>\n";
        return false;
    }
}
bool OcsServer::retrieveTextPageNo(std::string _POST[2], int clientSocket) {

    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open(m_DB_path, &DB)) {

        int tokno_start = safeStrToInt(_POST[0]);
        int tokno_end = safeStrToInt(_POST[1]);

        const char *sql_text;

        std::ostringstream html;
        writeTextBody(html, DB, tokno_start, tokno_end);      

        int content_length = html.str().size();
        std::ostringstream post_response_ss;
        post_response_ss << "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=UTF-8\r\nContent-Length: " << content_length << "\r\n\r\n" << html.str();
        int length = post_response_ss.str().size() + 1;
        sendToClient(clientSocket, post_response_ss.str().c_str(), length);

        sqlite3_close(DB);
        return true;

    }
    else {       
        
        std::cout << "Database connection failed on retrieveTextSubtitle()<br><br>\n";
        return false;
    }
}

void OcsServer::void_retrieveText(std::ostringstream &html, std::string& l_page_toknos_arr, Cookies& l_cookies) {
    //these should all have been corrected to sensible values by the insertTextSelect() function
    // int cookie_textselect = safeStrToInt(m_cookies[0]);
    // int cookie_pageno = safeStrToInt(m_cookies[1]);
    // int cookie_subtitle_id = safeStrToInt(m_cookies[2]);
    int cookie_textselect = safeStrToInt(l_cookies.text_id);
    int cookie_pageno = safeStrToInt(l_cookies.current_pageno);
    int cookie_subtitle_id = safeStrToInt(l_cookies.subtitle_id);

    int sentences_per_page = 30;
    std::ostringstream page_toknos_arr;
    page_toknos_arr << "[";
    int pageno_count = 1;
    int sentence_count = 0;

    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open(m_DB_path, &DB)) {

        const char *sql_text;

        sql_text = "SELECT tokno_start, tokno_end FROM subtitles WHERE subtitle_id = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, cookie_subtitle_id);
        sqlite3_step(statement);

        int tokno_start = sqlite3_column_int(statement, 0);
        int tokno_end = sqlite3_column_int(statement, 1);
        sqlite3_finalize(statement);

        //the sentence_nos are not in any guaranteed order
        sql_text = "SELECT sentence_no, tokno FROM corpus WHERE rowid >= ? AND rowid <= ? GROUP BY sentence_no ORDER BY tokno";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, tokno_start);
        sqlite3_bind_int(statement, 2, tokno_end);

        std::vector<int> page_toknos_vec;
        page_toknos_vec.reserve(16);
        page_toknos_vec.push_back(tokno_start);
        //I don't know whether the sentence_nos are guaranteed to be contiguous so I have to count each row rather than just do "last - first" to get the total number of sentences
        page_toknos_arr << "[" << tokno_start;
        while(sqlite3_step(statement) == SQLITE_ROW) {
            int sentence_no = sqlite3_column_int(statement, 0);
            int sentence_start_tokno = sqlite3_column_int(statement, 1);
            // sentence_count++;

            if(sentence_count > 0 && sentence_count % sentences_per_page == 0) {
                page_toknos_arr << "," << sentence_start_tokno - 1 << "],[" << sentence_start_tokno;
                page_toknos_vec.push_back(sentence_start_tokno);
                pageno_count++;
            }
            sentence_count++;
        }
        page_toknos_vec.push_back(tokno_end + 1);
        page_toknos_arr << "," << tokno_end << "]]";
        //m_page_toknos_arr = page_toknos_arr.str();
        l_page_toknos_arr = page_toknos_arr.str();
        
        //m_pageno_count = pageno_count;
        sqlite3_finalize(statement);

        if(cookie_pageno > pageno_count) {
            cookie_pageno = 1;
        }
        // m_cookies[1] = std::to_string(cookie_pageno);
        l_cookies.current_pageno = std::to_string(cookie_pageno);        

        html << "<br><div id=\"textbody\">";
        writeTextBody(html, DB, page_toknos_vec[cookie_pageno - 1], page_toknos_vec[cookie_pageno] - 1);

        html << "</div>";       

        if(pageno_count > 1) {
            html << "<br><br><div id='pageno_footer'><div id=\"pagenos\">";
            html << "<div id=\"pageno_leftarrow\" class=\"nav_arrow\">&lt;</div>";
            html << "<textarea id=\"pageno_box\" spellcheck=\"false\" autocomplete=\"off\">";
            html << l_cookies.current_pageno;
            html << "</textarea>";
            html << "<div class=\"pageno_text\" style=\"width: 40px;\">of</div>";
            html << "<div class=\"pageno_text\">" << pageno_count << "</div>";
            
            html << "<div id=\"pageno_rightarrow\" class=\"nav_arrow\">&gt;</div>";             
            html << "</div></div>";
        }
        else {
            html << "<br>";
        }
        std::cout << l_page_toknos_arr << "\n";

        sqlite3_close(DB); 

    }
    else {       
        
        html << "Database connection failed on void_retrieveText()<br><br>\n";
    }
    // html << "<br><br><div id=\"textbody\">text_id: " << cookie_textselect << "<br>subtitle_id: " << cookie_subtitle_id << "<br>pageno: " << cookie_pageno << "</div>\n";
}


void OcsServer::sendBinaryFile(char* url_c_str, int clientSocket, const std::string &content_type) {

    std::ifstream urlFile(url_c_str, std::ios::binary);
    if (urlFile.good())
    {
        std::cout << "This file exists and was opened successfully." << std::endl;

        struct stat size_result;    
        int font_filesize = 0;
        if(stat(url_c_str, &size_result) == 0) {
            font_filesize = size_result.st_size;
            std::cout << "Fontfile size: " << font_filesize << " bytes" << std::endl;
        }
        else {
            std::cout << "Error reading fontfile size" << std::endl;
            return;
        }
        std::string headers =  "HTTP/1.1 200 OK\r\nContent-Type: "+content_type+"\r\nContent-Length: "+ std::to_string(font_filesize) + "\r\n";
        if(content_type == "font/ttf") headers = headers + "Cache-Control: public, max-age=31536000, immutable\r\n";
        headers = headers + "\r\n";
        int headers_size = headers.size();
        const char* headers_c_str = headers.c_str();

        if(font_filesize < 1048576) {
            char content_buf[headers_size + font_filesize + 1];

            memcpy(content_buf, headers_c_str, headers_size); //.size() leaves off null-termination in its count

            urlFile.read(content_buf + headers_size, font_filesize);

            content_buf[headers_size + font_filesize] = '\0';

            sendToClient(clientSocket, content_buf, headers_size + font_filesize);

            urlFile.close();
        }
        else {
            char* content_buf = new char[headers_size + font_filesize + 1];

            memcpy(content_buf, headers_c_str, headers_size); //.size() leaves off null-termination in its count

            urlFile.read(content_buf + headers_size, font_filesize);

            content_buf[headers_size + font_filesize] = '\0';

            sendToClient(clientSocket, content_buf, headers_size + font_filesize);

            delete[] content_buf;
            urlFile.close();
        }      
    }
    else
    {
        std::cout << "This file was not opened successfully." << std::endl;
        sendToClient(clientSocket, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n", 46);
        return;
    }
    
}

//it's simpler to handle errors and timeouts if I keep everything as raw text until it gets back to the client; pretty sure having an xhttp.reponseType = 'json' just means it runs JSON.parse() implicitly rather than explicitly;
bool OcsServer::curlLookup(std::string _POST[1], int clientSocket) {
    
    CurlFetcher query(_POST[0].c_str(), m_dict_cookies);
    query.fetch();
    
    std::ostringstream post_response;
    int content_length = query.m_get_html.size();
    post_response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " << content_length << "\r\n\r\n" << query.m_get_html;

    int length = post_response.str().size() + 1;

    sendToClient(clientSocket, post_response.str().c_str(), length);
    return true;
}

bool OcsServer::retrieveTextFromSearch(std::string _POST[1], int clientSocket) {

    int result_tokno = safeStrToInt(_POST[0]);

    int sentences_per_page = 30;
    std::ostringstream page_toknos_arr;
    page_toknos_arr << "[";
    int pageno_count = 1;
    int sentence_count = 0;
    bool pageno_found = false;

    sqlite3* DB;
    sqlite3_stmt* statement;

    if (!sqlite3_open(m_DB_path, &DB)) {

        const char* sql_text;

        sql_text = "SELECT tokno_start, tokno_end, subtitle_id FROM subtitles WHERE tokno_start <= ? AND tokno_end >= ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, result_tokno);
        sqlite3_bind_int(statement, 2, result_tokno);
        sqlite3_step(statement);
        int tokno_start = sqlite3_column_int(statement, 0);
        int tokno_end = sqlite3_column_int(statement, 1);
        int selected_subtitle_id = sqlite3_column_int(statement, 2);
        sqlite3_finalize(statement);

        sql_text = "SELECT text_id, text_title FROM texts WHERE tokno_start <= ? AND tokno_end >= ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, result_tokno);
        sqlite3_bind_int(statement, 2, result_tokno);
        sqlite3_step(statement);
        int text_id = sqlite3_column_int(statement, 0);
        std::string text_title_str = (const char*)sqlite3_column_text(statement, 1);
        sqlite3_finalize(statement);

        sql_text = "SELECT subtitle_text, subtitle_id, tokno_start, tokno_end FROM subtitles WHERE text_id = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, text_id);
        std::ostringstream subtitles_html;
        std::string subtitle_text_str;
        int subtitle_tokno_start, subtitle_tokno_end, subtitle_id;
        while (sqlite3_step(statement) == SQLITE_ROW) {
            subtitle_text_str.assign((const char*)sqlite3_column_text(statement, 0));
            subtitle_id = sqlite3_column_int(statement, 1);
            subtitle_tokno_start = sqlite3_column_int(statement, 2);
            subtitle_tokno_end = sqlite3_column_int(statement, 3);
            subtitles_html << "<option value=\"" << subtitle_id << "\" data-tokno_start=\"" << subtitle_tokno_start << "\" data-tokno_end=\"" << subtitle_tokno_end << "\"";
            if(subtitle_id == selected_subtitle_id) subtitles_html << " selected";
            subtitles_html << ">" << htmlspecialchars(subtitle_text_str) << "</option>";
        }
        sqlite3_finalize(statement);

        //the sentence_nos are not in any guaranteed order
        sql_text = "SELECT sentence_no, tokno FROM corpus WHERE rowid >= ? AND rowid <= ? GROUP BY sentence_no ORDER BY tokno";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, tokno_start);
        sqlite3_bind_int(statement, 2, tokno_end);

        std::vector<int> page_toknos_vec;
        page_toknos_vec.reserve(16);
        page_toknos_vec.push_back(tokno_start);
        //I don't know whether the sentence_nos are guaranteed to be contiguous so I have to count each row rather than just do "last - first" to get the total number of sentences
        page_toknos_arr << "[" << tokno_start;
        while (sqlite3_step(statement) == SQLITE_ROW) {
            int sentence_no = sqlite3_column_int(statement, 0);
            int sentence_start_tokno = sqlite3_column_int(statement, 1);
            // sentence_count++;

            if (sentence_count > 0 && sentence_count % sentences_per_page == 0) {
                page_toknos_arr << "," << sentence_start_tokno - 1 << "],[" << sentence_start_tokno;
                page_toknos_vec.push_back(sentence_start_tokno);
                pageno_count++;
                
            }
            sentence_count++;
        }
        int result_pageno = 0;
        for(const int page_tokno_start : page_toknos_vec) { 
            if(result_tokno < page_tokno_start) break;
            result_pageno++;
        }

        std::cout << "result_page_no: " << result_pageno << "\n";

        page_toknos_vec.push_back(tokno_end + 1);
        page_toknos_arr << "," << tokno_end << "]]";
        //m_page_toknos_arr = page_toknos_arr.str();
        //m_pageno_count = pageno_count;
        sqlite3_finalize(statement);

        //m_cookies[1] = std::to_string(result_pageno);

        std::ostringstream html;
        html << "<br><div id=\"textbody\">";
        writeTextBody(html, DB, page_toknos_vec[result_pageno - 1], page_toknos_vec[result_pageno] - 1);

        html << "</div>";

        if (pageno_count > 1) {
            html << "<br><br><div id='pageno_footer'><div id=\"pagenos\">";
            html << "<div id=\"pageno_leftarrow\" class=\"nav_arrow\">&lt;</div>";
            html << "<textarea id=\"pageno_box\" spellcheck=\"false\" autocomplete=\"off\">";
            html << result_pageno;
            html << "</textarea>";
            html << "<div class=\"pageno_text\" style=\"width: 40px;\">of</div>";
            html << "<div class=\"pageno_text\">" << pageno_count << "</div>";

            html << "<div id=\"pageno_rightarrow\" class=\"nav_arrow\">&gt;</div>";
            html << "</div></div>";
        }
        else {
            html << "<br>";
        }
        //std::cout << m_page_toknos_arr << "\n";

        std::string json_str = "{\"html\":\"" + escapeQuotes(html.str()) + "\",\"subtitles_html\":\"" + escapeQuotes(subtitles_html.str()) + "\",\"page_toknos\":" + page_toknos_arr.str() + ",\"text_id\":" + std::to_string(text_id) + ",\"result_pageno\":" + std::to_string(result_pageno) + ",\"subtitle_id\":" + std::to_string(selected_subtitle_id) + "}";
        int content_length = json_str.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);

        sqlite3_close(DB);
        return true;

    }
    else {

        std::cout << "Database connection failed on retrieveTextFromSearch()<br><br>\n";
        return false;
    }
    // html << "<br><br><div id=\"textbody\">text_id: " << cookie_textselect << "<br>subtitle_id: " << cookie_subtitle_id << "<br>pageno: " << cookie_pageno << "</div>\n";
}

bool OcsServer::lcsRegexSearch(std::string _POST[3], int clientSocket) {
    sqlite3* DB;
    
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString regex_str = URIDecode(_POST[0]).c_str();
    icu::RegexMatcher *matcher = new icu::RegexMatcher(regex_str, 0, status);

    if(U_FAILURE(status)) {
        std::cout << "the regex failed\n";
        std::string json_str = "{\"error\":\"That regex was malformed. Repent!\"}";
        int content_length = json_str.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
        return true;
    }
  
    
    if(!sqlite3_open(m_DB_path, &DB)) {
        

        sqlite3_stmt* titles_stmt;
        const char* sql_titles_select = "SELECT text_title, tokno_start, tokno_end FROM texts";
        sqlite3_prepare_v2(DB, sql_titles_select, -1, &titles_stmt, NULL);

        std::ostringstream text_title_info;
        std::string text_title;
        int title_tokno_start = 0;
        int title_tokno_end = 0;
        text_title_info << '[';
        while(sqlite3_step(titles_stmt) == SQLITE_ROW) {
            text_title_info << "[\"" << (const char*)sqlite3_column_text(titles_stmt, 0) << "\",";
            text_title_info << sqlite3_column_int(titles_stmt, 1) << ',' << sqlite3_column_int(titles_stmt, 2) << "],";
        }
        text_title_info.seekp(-1, std::ios_base::cur);
        text_title_info << ']';
        sqlite3_finalize(titles_stmt);

        sqlite3_stmt* statement1;
        sqlite3_stmt* statement2;     
       
        std::string regex_bytes;
        regex_str.toUTF8String(regex_bytes);
        std::cout << "regex: " << regex_bytes << "\n";

        int tokno_start = safeStrToInt(_POST[1]);
        int tokno_end = safeStrToInt(_POST[2]);

        const char* sql_text1 = "SELECT tokno, autoreconstructed_lcs, sentence_no FROM corpus WHERE tokno >= ? AND tokno <= ? AND autoreconstructed_lcs IS NOT NULL";
        const char* sql_text2 = "SELECT tokno, chu_word_torot, presentation_before, presentation_after FROM corpus WHERE tokno < ? AND tokno > ? AND sentence_no = ?";
        sqlite3_prepare_v2(DB, sql_text1, -1, &statement1, NULL);
        sqlite3_prepare_v2(DB, sql_text2, -1, &statement2, NULL);
         
        sqlite3_bind_int(statement1, 1, tokno_start);
        sqlite3_bind_int(statement1, 2, tokno_end);

        std::ostringstream lcs_results;
        lcs_results << "[";
        std::ostringstream text_results;
        text_results << "[";
        std::ostringstream tokno_results;
        tokno_results << "[";


        int tokno = 0;
        icu::UnicodeString lcs_result;
        std::string lcs_result_str = "";
        std::string chu_word_torot = "";
        std::string present_before = "";
        std::string presentation_after = "";

        int sentence_no = 0;
        int results_count = 0;
   
        while(sqlite3_step(statement1) == SQLITE_ROW) {
           
            //lcs_result = (const char*)sqlite3_column_text(statement1, 1);

            lcs_result.setTo((const char*)sqlite3_column_text(statement1, 1));
            matcher->reset(lcs_result);
            if(matcher->find()) {
                
                lcs_result.toUTF8String(lcs_result_str);

                sentence_no = sqlite3_column_int(statement1, 2);
                tokno = sqlite3_column_int(statement1, 0);
                //std::cout << "lcs_result: " << lcs_result << "\n";
                lcs_results << "\"" << htmlspecialchars(lcs_result_str) << "\"" << ",";
                tokno_results << tokno << ",";

                std::ostringstream text_content_result_oss;

                sqlite3_bind_int(statement2, 1, tokno + 5);
                sqlite3_bind_int(statement2, 2, tokno - 5);
                sqlite3_bind_int(statement2, 3, sentence_no);
                while(sqlite3_step(statement2) == SQLITE_ROW) {
                    text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 2));
                    if(sqlite3_column_int(statement2, 0) == tokno) {
                        text_content_result_oss << "<span class=\"result_word\">" + applySuperscripts((const char*)sqlite3_column_text(statement2, 1)) + "</span>";
                    }
                    else text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 1));
                    
                    text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 3));
                }
                text_results << "\"" << escapeQuotes(text_content_result_oss.str()) << "\",";

                lcs_result_str.clear();
                sqlite3_reset(statement2);
                sqlite3_clear_bindings(statement2);
                results_count++;

                if(results_count > 5000) {

                    std::cout << "too many results\n";
                    std::string json_str = "{\"error\":\"Too many results, please narrow the search query\"}";
                    int content_length = json_str.size();

                    std::ostringstream post_response;
                    post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

                    int length = post_response.str().size() + 1;
                    sendToClient(clientSocket, post_response.str().c_str(), length);

                    sqlite3_finalize(statement1);
                    sqlite3_finalize(statement2);

                    sqlite3_close(DB);
                    return true;
                }
            }
        }
        sqlite3_finalize(statement1);
        sqlite3_finalize(statement2);
                
        if(results_count > 0) {
            lcs_results.seekp(-1, std::ios_base::cur);
            text_results.seekp(-1, std::ios_base::cur);
            tokno_results.seekp(-1, std::ios_base::cur);
        }

        lcs_results << "]";
        text_results << "]";
        tokno_results << "]";    

        std::string json_str = "[" + lcs_results.str() + "," + text_results.str() + "," + tokno_results.str() + "," + text_title_info.str() + "]";
        int content_length = json_str.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
        sqlite3_close(DB);
       
        return true;
    }
    else {
        std::cout << "Database connection failed in lcsRegexSearch()" << std::endl;
        return false;
    }
}

bool OcsServer::greekTooltips(std::string _POST[2], int clientSocket) {
    sqlite3* DB;    
    
    if(!sqlite3_open(m_DB_path, &DB)) {

        sqlite3_stmt* statement1;
        sqlite3_stmt* statement2;     

        int tokno_start = safeStrToInt(_POST[0]);
        int tokno_end = safeStrToInt(_POST[1]);
        

        const char* sql_text1 = "SELECT chu_tokno, gk_word, gk_lemma_id, gk_morph_tag, gk_pos FROM greek_alignments WHERE chu_tokno >= ? AND chu_tokno <= ?";
        std::cout << sql_text1 << "\n";
        std::cout << "prep code: " << sqlite3_prepare_v2(DB, sql_text1, -1, &statement1, NULL) << "\n";
        std::cout << "bind code: " << sqlite3_bind_int(statement1, 1, tokno_start) << "\n";
        sqlite3_bind_int(statement1, 2, tokno_end);

        const char* sql_text2 = "SELECT gk_lemma_form FROM greek_lemmas WHERE gk_lemma_id = ?";
        sqlite3_prepare_v2(DB, sql_text2, -1, &statement2, NULL);


        std::string gk_word, gk_morph_tag, gk_lemma_form;
        int chu_tokno, gk_lemma_id, gk_pos;

        std::ostringstream json;
        json << "{";
        
        int results_count = 0;
        while(sqlite3_step(statement1) == SQLITE_ROW) {
            chu_tokno = sqlite3_column_int(statement1, 0);
            gk_lemma_id = sqlite3_column_int(statement1, 2);
            gk_pos = sqlite3_column_int(statement1, 4);
            gk_morph_tag = (const char*)sqlite3_column_text(statement1, 3);
            gk_word = (const char*)sqlite3_column_text(statement1, 1);

            sqlite3_bind_int(statement2, 1, gk_lemma_id);
            sqlite3_step(statement2);
            gk_lemma_form = (const char*)sqlite3_column_text(statement2, 0);

            json << "\"" << chu_tokno << "\":[\"" << escapeQuotes(gk_word) << "\",\"" << escapeQuotes(gk_lemma_form) << "\",\"" << gk_morph_tag << "\"," << gk_pos << "],";


            sqlite3_reset(statement2);
            sqlite3_clear_bindings(statement2);

            results_count++;
        }
        sqlite3_finalize(statement1);
        sqlite3_finalize(statement2);
        
        if(results_count > 0) json.seekp(-1, std::ios_base::cur);
        json << "}";

        std::string json_str = json.str();
        int content_length = json_str.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
        sqlite3_close(DB);
       
        return true;
    }
    else {
        std::cout << "Database connection failed in greekTooltips()" << std::endl;
        return false;
    }
}

bool OcsServer::generateInflection(std::string _POST[3], int clientSocket) {
    std::string stem = _POST[0];
    std::string conj_type = _POST[1];
    bool noun_verb = _POST[2] == "2" ? true : false;


    std::ostringstream post_response;
    std::ostringstream json_response;
    
    //since the inflection-tables are static they should be initialised only once on startup not for each class instance
    LcsFlecter lcs_flecter({stem, conj_type, noun_verb});

    json_response << "[";
    int i = 0;
    for(const auto& inflections : lcs_flecter.getFullParadigm()) {
        json_response << "{";
        int j = 0;
        for(const auto& inflection : inflections) {
            //std::cout << inflection.desinence_ix << " " << inflection.flected_form << "\n";
            json_response << '\"' << inflection.desinence_ix << "\":\"" << inflection.flected_form << "\",";
            j++;
        }
        if(j > 0) json_response.seekp(-1, std::ios_base::cur);
        json_response << "},";
        //std::cout << "\n";
        i++;
    }
    if(i > 0) json_response.seekp(-1, std::ios_base::cur);
    json_response << "]";


    int content_length = json_response.str().length();
    post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_response.str();

    
    sendToClient(clientSocket, post_response.str().c_str(), post_response.str().length() + 1);

    return true;

}

void OcsServer::writeSearchResults(const std::vector<int> &result_toknos_vec, sqlite3_stmt *statement1, sqlite3_stmt *statement2, std::ostringstream &text_results, std::ostringstream& lcs_results, std::ostringstream& tokno_results){
    std::string lcs_result;
    std::string chu_word_torot;
    std::string presentation_before;
    std::string presentation_after;

    std::ostringstream text_content_result_oss;

    for(const int& result_tokno : result_toknos_vec) {
        sqlite3_bind_int(statement1, 1, result_tokno);
        sqlite3_step(statement1);
        const unsigned char* raw_lcs = sqlite3_column_text(statement1, 0);
        if(raw_lcs != nullptr) lcs_result = (const char*)raw_lcs;
        int sentence_no = sqlite3_column_int(statement1, 1);

        // std::cout << "lcs_result: " << lcs_result << "\n";
        // std::cout << "sentence_no: " << sentence_no << "\n";

        sqlite3_reset(statement1);
        sqlite3_clear_bindings(statement1);

        lcs_results << "\"" << lcs_result << "\"" << ",";
        tokno_results << result_tokno << ",";

        text_content_result_oss.str("");
        sqlite3_bind_int(statement2, 1, sentence_no);
        sqlite3_bind_int(statement2, 2, result_tokno + 5);
        sqlite3_bind_int(statement2, 3, result_tokno - 5);

        while(sqlite3_step(statement2) == SQLITE_ROW) {
            text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 2));
            if(sqlite3_column_int(statement2, 0) == result_tokno) {
                text_content_result_oss << "<span class=\"result_word\">" + applySuperscripts((const char*)sqlite3_column_text(statement2, 1)) + "</span>";
            }
            else text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 1));
            
            text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 3));
        }
        text_results << "\"" << escapeQuotes(text_content_result_oss.str()) << "\",";

        sqlite3_reset(statement2);
        sqlite3_clear_bindings(statement2);
    }

}

void OcsServer::trigramSearch(sqlite3* db_connection, const std::string& trigram_table, const std::string& target_column, std::string query, int tokno_start, int tokno_end, int clientSocket) {
        std::cout << "PRAGMA return_code: " << sqlite3_exec(db_connection, "PRAGMA case_sensitive_like = ON", nullptr, nullptr, nullptr) << "\n";

        //start by getting the text_title information so we can say which text the results come from on the front-end (wasteful to do it every search but this way will stop us having to update Javascript whenever the database changes)

        sqlite3_stmt* titles_stmt;
        const char* sql_titles_select = "SELECT text_title, tokno_start, tokno_end FROM texts";
        sqlite3_prepare_v2(db_connection, sql_titles_select, -1, &titles_stmt, NULL);

        std::ostringstream text_title_info;
        std::string text_title;
        int title_tokno_start = 0;
        int title_tokno_end = 0;
        text_title_info << '[';
        while(sqlite3_step(titles_stmt) == SQLITE_ROW) {
            text_title_info << "[\"" << (const char*)sqlite3_column_text(titles_stmt, 0) << "\",";
            text_title_info << sqlite3_column_int(titles_stmt, 1) << ',' << sqlite3_column_int(titles_stmt, 2) << "],";
        }
        text_title_info.seekp(-1, std::ios_base::cur);
        text_title_info << ']';
        sqlite3_finalize(titles_stmt);

        icu::UnicodeString unicode_query = query.c_str();
        int32_t num_chars = unicode_query.countChar32();

        std::vector<int> result_toknos_vec;
        result_toknos_vec.reserve(256);

        sqlite3_stmt* trigram_stmt;
        std::string sql_trigram_select;
        if(num_chars < 3) {
            sql_trigram_select = "SELECT tokno FROM "+trigram_table+" WHERE tokno >= ? AND tokno <= ? AND trigram LIKE ?";
            query = query.append("%");

            sqlite3_prepare_v2(db_connection, sql_trigram_select.c_str(), -1, &trigram_stmt, NULL);
            sqlite3_bind_int(trigram_stmt, 1, tokno_start);
            sqlite3_bind_int(trigram_stmt, 2, tokno_end);
            sqlite3_bind_text(trigram_stmt, 3, query.c_str(), -1, SQLITE_STATIC);
            int prev_tokno = 0;
            while(sqlite3_step(trigram_stmt) == SQLITE_ROW) {
                int result_tokno = sqlite3_column_int(trigram_stmt, 0);
                if(result_tokno != prev_tokno) {
                    result_toknos_vec.emplace_back(result_tokno);
                }
            }
            sqlite3_finalize(trigram_stmt);
        }
        else {
            sql_trigram_select = "SELECT tokno FROM "+trigram_table+" WHERE tokno >= ? AND tokno <= ? AND trigram = ?";
            sqlite3_prepare_v2(db_connection, sql_trigram_select.c_str(), -1, &trigram_stmt, NULL);

            std::string query_trigram;
            int32_t trigram_start_offset = 0;

            int32_t num_of_trigrams = num_chars - 2;
            std::unordered_map<int, int> tokno_trigram_matches_map;
            tokno_trigram_matches_map.reserve(512);
            while(trigram_start_offset + 2 < num_chars) {
                sqlite3_bind_int(trigram_stmt, 1, tokno_start);
                sqlite3_bind_int(trigram_stmt, 2, tokno_end);
                unicode_query.tempSubString(trigram_start_offset, 3).toUTF8String(query_trigram);
                sqlite3_bind_text(trigram_stmt, 3, query_trigram.c_str(), -1, SQLITE_STATIC);

                int result_tokno = 0;
                while(sqlite3_step(trigram_stmt) == SQLITE_ROW) {
                    int new_result_tokno = sqlite3_column_int(trigram_stmt, 0);
                    if(result_tokno == new_result_tokno) continue; //this prevents double-matches for words which contain the same trigram more than once
                    result_tokno = new_result_tokno;
                    if(tokno_trigram_matches_map.contains(result_tokno)) {
                        tokno_trigram_matches_map[result_tokno]++;
                    }
                    else {
                        tokno_trigram_matches_map.emplace(result_tokno, 1);
                    }
                }
                sqlite3_reset(trigram_stmt);
                sqlite3_clear_bindings(trigram_stmt);

                query_trigram.clear();
                trigram_start_offset++;
            }
            sqlite3_finalize(trigram_stmt);

            for(const auto& tokno_trigram_match : tokno_trigram_matches_map) {
                if(tokno_trigram_match.second == num_of_trigrams) {
                    result_toknos_vec.emplace_back(tokno_trigram_match.first);
                }
            }
        }

        int results_count = result_toknos_vec.size();

        if(results_count > 5000) {

            std::cout << "too many results\n";
            std::string json_str = "{\"error\":\"Too many results, please narrow the search query\"}";
            int content_length = json_str.size();

            std::ostringstream post_response;
            post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

            int length = post_response.str().size() + 1;
            sendToClient(clientSocket, post_response.str().c_str(), length);
            return;
        }


        sqlite3_stmt* statement1;
        sqlite3_stmt* statement2;     

        std::string sql_text1 = "SELECT "+target_column+", sentence_no FROM corpus WHERE tokno = ?";
        const char* sql_text2 = "SELECT tokno, chu_word_torot, presentation_before, presentation_after FROM corpus WHERE sentence_no = ? AND tokno <= ? AND tokno >= ?";
        sqlite3_prepare_v2(db_connection, sql_text1.c_str(), -1, &statement1, NULL);
        sqlite3_prepare_v2(db_connection, sql_text2, -1, &statement2, NULL);
        

        std::ostringstream lcs_results;
        lcs_results << "[";
        std::ostringstream text_results;
        text_results << "[";
        std::ostringstream tokno_results;
        tokno_results << "[";

        std::sort(result_toknos_vec.begin(), result_toknos_vec.end());

        writeSearchResults(result_toknos_vec, statement1, statement2, text_results, lcs_results, tokno_results);
    
        sqlite3_finalize(statement1);
        sqlite3_finalize(statement2);
                
        if(results_count > 0) {
            lcs_results.seekp(-1, std::ios_base::cur);
            text_results.seekp(-1, std::ios_base::cur);
            tokno_results.seekp(-1, std::ios_base::cur);
        }

        lcs_results << "]";
        text_results << "]";
        tokno_results << "]";    

        std::string json_str = "[" + lcs_results.str() + "," + text_results.str() + "," + tokno_results.str() + "," + text_title_info.str() + "]";
        int content_length = json_str.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
}

bool OcsServer::lcsTrigramSearch(std::string _POST[3], int clientSocket) {
    //this is a trick to get wildcarded (LIKE '%query%') SELECT statements to use a database-index, which ordinarily they can't do, because otherwise such queries over the full corpus would scale linearly with corpus-size
    sqlite3* DB;

    if(!sqlite3_open(m_DB_path, &DB)) {
        
        std::cout << "PRAGMA return_code: " << sqlite3_exec(DB, "PRAGMA case_sensitive_like = ON", nullptr, nullptr, nullptr) << "\n";

        std::string query = URIDecode(_POST[0]);
        int tokno_start = safeStrToInt(_POST[1]);
        int tokno_end = safeStrToInt(_POST[2]);

        //start by getting the text_title information so we can say which text the results come from on the front-end (wasteful to do it every search but this way will stop us having to update Javascript whenever the database changes)

        sqlite3_stmt* titles_stmt;
        const char* sql_titles_select = "SELECT text_title, tokno_start, tokno_end FROM texts";
        sqlite3_prepare_v2(DB, sql_titles_select, -1, &titles_stmt, NULL);

        std::ostringstream text_title_info;
        std::string text_title;
        int title_tokno_start = 0;
        int title_tokno_end = 0;
        text_title_info << '[';
        while(sqlite3_step(titles_stmt) == SQLITE_ROW) {
            text_title_info << "[\"" << (const char*)sqlite3_column_text(titles_stmt, 0) << "\",";
            text_title_info << sqlite3_column_int(titles_stmt, 1) << ',' << sqlite3_column_int(titles_stmt, 2) << "],";
        }
        text_title_info.seekp(-1, std::ios_base::cur);
        text_title_info << ']';
        sqlite3_finalize(titles_stmt);

        icu::UnicodeString unicode_query = query.c_str();
        int32_t num_chars = unicode_query.countChar32();

        std::vector<int> result_toknos_vec;
        result_toknos_vec.reserve(256);

        sqlite3_stmt* trigram_stmt;
        const char* sql_trigram_select;
        if(num_chars < 3) {
            sql_trigram_select = "SELECT tokno FROM lcs_trigrams WHERE tokno >= ? AND tokno <= ? AND trigram LIKE ?";
            query = query.append("%");

            sqlite3_prepare_v2(DB, sql_trigram_select, -1, &trigram_stmt, NULL);
            sqlite3_bind_int(trigram_stmt, 1, tokno_start);
            sqlite3_bind_int(trigram_stmt, 2, tokno_end);
            sqlite3_bind_text(trigram_stmt, 3, query.c_str(), -1, SQLITE_STATIC);
            int prev_tokno = 0;
            while(sqlite3_step(trigram_stmt) == SQLITE_ROW) {
                int result_tokno = sqlite3_column_int(trigram_stmt, 0);
                if(result_tokno != prev_tokno) {
                    result_toknos_vec.emplace_back(result_tokno);
                }
            }
            sqlite3_finalize(trigram_stmt);
        }
        else {
            sql_trigram_select = "SELECT tokno, rowid FROM lcs_trigrams WHERE tokno >= ? AND tokno <= ? AND trigram = ?"; // ORDER BY tokno but should do it anyway
            sqlite3_prepare_v2(DB, sql_trigram_select, -1, &trigram_stmt, NULL);

            std::string query_trigram;
            int32_t trigram_start_offset = 0;

            int32_t num_of_trigrams = num_chars - 2;
            std::unordered_map<int, int> tokno_trigram_matches_map;
            tokno_trigram_matches_map.reserve(512);
            std::unordered_set<int> counted_trigram_rowids;
            counted_trigram_rowids.reserve(512);
            while(trigram_start_offset + 2 < num_chars) {
                sqlite3_bind_int(trigram_stmt, 1, tokno_start);
                sqlite3_bind_int(trigram_stmt, 2, tokno_end);
                unicode_query.tempSubString(trigram_start_offset, 3).toUTF8String(query_trigram);
                sqlite3_bind_text(trigram_stmt, 3, query_trigram.c_str(), -1, SQLITE_STATIC);

                int result_tokno = 0;
                while(sqlite3_step(trigram_stmt) == SQLITE_ROW) {
                    int new_result_tokno = sqlite3_column_int(trigram_stmt, 0);
                    if(result_tokno == new_result_tokno) continue; //this prevents double-matches for words which contain the same trigram more than once
                    
                    int new_rowid = sqlite3_column_int(trigram_stmt, 1); //this prevents cases like "golgol" from matching "golgoĺ" by stopping the second "gol" in the query from rematching the first "gol" trigram and thus increasing the number of trigram-matches to 4 (== match) when it should be 3
                    if(counted_trigram_rowids.contains(new_rowid)) {
                        continue;
                    }
                    else {
                        counted_trigram_rowids.insert(new_rowid);
                    }

                    result_tokno = new_result_tokno;
                    if(tokno_trigram_matches_map.contains(result_tokno)) {
                        tokno_trigram_matches_map[result_tokno]++;
                    }
                    else {
                        tokno_trigram_matches_map.emplace(result_tokno, 1);
                    }
                }
                sqlite3_reset(trigram_stmt);
                sqlite3_clear_bindings(trigram_stmt);

                query_trigram.clear();
                trigram_start_offset++;
            }
            sqlite3_finalize(trigram_stmt);

            for(const auto& tokno_trigram_match : tokno_trigram_matches_map) {
                if(tokno_trigram_match.second == num_of_trigrams) {
                    result_toknos_vec.emplace_back(tokno_trigram_match.first);
                }
            }
        }

        int results_count = result_toknos_vec.size();

        if(results_count > 5000) {

            std::cout << "too many results\n";
            std::string json_str = "{\"error\":\"Too many results, please narrow the search query\"}";
            int content_length = json_str.size();

            std::ostringstream post_response;
            post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

            int length = post_response.str().size() + 1;
            sendToClient(clientSocket, post_response.str().c_str(), length);

            sqlite3_close(DB);
            return true;
        }


        sqlite3_stmt* statement1;
        sqlite3_stmt* statement2;     

        const char* sql_text1 = "SELECT autoreconstructed_lcs, sentence_no FROM corpus WHERE tokno = ?";
        const char* sql_text2 = "SELECT tokno, chu_word_torot, presentation_before, presentation_after FROM corpus WHERE sentence_no = ? AND tokno <= ? AND tokno >= ?";
        sqlite3_prepare_v2(DB, sql_text1, -1, &statement1, NULL);
        sqlite3_prepare_v2(DB, sql_text2, -1, &statement2, NULL);
        

        std::ostringstream lcs_results;
        lcs_results << "[";
        std::ostringstream text_results;
        text_results << "[";
        std::ostringstream tokno_results;
        tokno_results << "[";

        std::sort(result_toknos_vec.begin(), result_toknos_vec.end());

        writeSearchResults(result_toknos_vec, statement1, statement2, text_results, lcs_results, tokno_results);
        /*std::string lcs_result = "";
        std::string chu_word_torot = "";
        std::string present_before = "";
        std::string presentation_after = "";

        for(const int& result_tokno : result_toknos_vec) {
            sqlite3_bind_int(statement1, 1, result_tokno);
            sqlite3_step(statement1);
            const unsigned char* raw_lcs = sqlite3_column_text(statement1, 0);
            if(raw_lcs != nullptr) lcs_result = (const char*)raw_lcs;
            int sentence_no = sqlite3_column_int(statement1, 1);

            // std::cout << "lcs_result: " << lcs_result << "\n";
            // std::cout << "sentence_no: " << sentence_no << "\n";

            sqlite3_reset(statement1);
            sqlite3_clear_bindings(statement1);

            lcs_results << "\"" << lcs_result << "\"" << ",";
            tokno_results << result_tokno << ",";

            std::ostringstream text_content_result_oss;
            sqlite3_bind_int(statement2, 1, sentence_no);
            sqlite3_bind_int(statement2, 2, result_tokno + 5);
            sqlite3_bind_int(statement2, 3, result_tokno - 5);

            while(sqlite3_step(statement2) == SQLITE_ROW) {
                text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 2));
                if(sqlite3_column_int(statement2, 0) == result_tokno) {
                    text_content_result_oss << "<span class=\"result_word\">" + applySuperscripts((const char*)sqlite3_column_text(statement2, 1)) + "</span>";
                }
                else text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 1));
                
                text_content_result_oss << applySuperscripts((const char*)sqlite3_column_text(statement2, 3));
            }
            text_results << "\"" << escapeQuotes(text_content_result_oss.str()) << "\",";

            sqlite3_reset(statement2);
            sqlite3_clear_bindings(statement2);
        } */
        sqlite3_finalize(statement1);
        sqlite3_finalize(statement2);
                
        if(results_count > 0) {
            lcs_results.seekp(-1, std::ios_base::cur);
            text_results.seekp(-1, std::ios_base::cur);
            tokno_results.seekp(-1, std::ios_base::cur);
        }

        lcs_results << "]";
        text_results << "]";
        tokno_results << "]";    

        std::string json_str = "[" + lcs_results.str() + "," + text_results.str() + "," + tokno_results.str() + "," + text_title_info.str() + "]";
        int content_length = json_str.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
        sqlite3_close(DB);
       
        return true;
    }
    else {
        std::cout << "Database connection failed in lcsTrigramSearch()" << std::endl;
        return false;
    }
}

bool OcsServer::cyrFuzzySearch(std::string _POST[3], int clientSocket) {
    sqlite3* DB;
    if(!sqlite3_open(m_DB_path, &DB)) {
        std::string query = URIDecode(_POST[0]);
        int tokno_start = safeStrToInt(_POST[1]);
        int tokno_end = safeStrToInt(_POST[2]);     
        trigramSearch(DB, "chu_normalised_trigrams", "chu_word_torot", query, tokno_start, tokno_end, clientSocket);
        sqlite3_close(DB);
        
        return true;
    }
    else {
        std::cout << "Database connection failed in cyrFuzzySearch()" << std::endl;
        return false;
    }
}

bool OcsServer::cyrSearch(std::string _POST[3], int clientSocket) {
    sqlite3* DB;
    if(!sqlite3_open(m_DB_path, &DB)) {
        std::string query = URIDecode(_POST[0]);
        int tokno_start = safeStrToInt(_POST[1]);
        int tokno_end = safeStrToInt(_POST[2]);     
        trigramSearch(DB, "chu_word_clean_trigrams", "chu_word_torot", query, tokno_start, tokno_end, clientSocket);
        sqlite3_close(DB);
        
        return true;
    }
    else {
        std::cout << "Database connection failed in cyrSearch()" << std::endl;
        return false;
    }
}

bool OcsServer::gorazdLookup(std::string _POST[2], int clientSocket) {

    int torot_lemma_id = safeStrToInt(_POST[0]);
    std::string plain_query = "";
    std::string uri_encoded_query = "";
    if(torot_lemma_id == 0) {
        uri_encoded_query = _POST[1];
        plain_query = URIDecode(_POST[1]);

        std::string json_str = curlGorazd(plain_query, uri_encoded_query);
        int content_length = json_str.size();
        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length); 
        return true;
    }
    //if we've already been passed the query-string from the client we won't even open the DB because we'll already have returned by now

    sqlite3* DB;
    if(!sqlite3_open(m_DB_path, &DB)) {
        
        sqlite3_stmt* statement;
        const char* sql_text = "SELECT lemma_ocs FROM lemmas WHERE lemma_id = ?";

        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, torot_lemma_id);
        sqlite3_step(statement);

        
        std::ostringstream uri_encoded_lemma_form_oss;
        const unsigned char* unsigned_char_lemma_form = sqlite3_column_text(statement, 0);
        const unsigned char* p = unsigned_char_lemma_form;
        if(unsigned_char_lemma_form != nullptr) {

            //this is encodeURIComponent() except without checking reserved characters that shouldn't be encoded      
            while(*p != '\0') {
                uri_encoded_lemma_form_oss << '%' << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(*p);
                ++p;
            }    
            plain_query = (const char*)unsigned_char_lemma_form;
        }

        std::string uri_encoded_lemma_form = uri_encoded_lemma_form_oss.str();
        //check for and remove TOROT #1, #2 etc. from end of disambiguated lemmas
        if(uri_encoded_lemma_form.find("%23") != std::string::npos) {
            uri_encoded_lemma_form = uri_encoded_lemma_form.substr(0, uri_encoded_lemma_form.size() - 6);
            plain_query = plain_query.substr(0, plain_query.size() - 2);
        }
        std::cout << "torot_lemma_form: " << plain_query << " | uri_encoded_lemma_form: " << uri_encoded_lemma_form_oss.str() << "\n";
        sqlite3_finalize(statement);       

        std::string json_str = curlGorazd(plain_query, uri_encoded_lemma_form);
        int content_length = json_str.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
        sqlite3_close(DB);
       
        return true;
    }
    else {
        std::cout << "Database connection failed in gorazdLookup()" << std::endl;
        return false;
    }
}

// std::string OcsServer::curlGorazd(std::string plain_query, std::string uri_encoded_query) {
//     std::string gorazd_query_url = "http://castor.gorazd.org:8080/gorazd/advanced_search;jsessionid=013373AF9710B0E8728F91826ABBD8DD?queryFields=%7B%221%22%3A%7B%22fieldName%22%3A%22HeaderAll%22%2C%22rawFieldQuery%22%3A%22" + uri_encoded_query + "%22%2C%22logTerm%22%3A%22%22%7D%7D";
    
//     std::cout << "gorazd_query_url: " << gorazd_query_url << "\n";
    
//     CurlFetcher query(gorazd_query_url.c_str());

//     query.fetch();

//     if(query.m_get_html.find("\"found\":0,") != std::string::npos) {
//         //there can be instances where SJS has no numeral appended, but SNSP does (родьство), which means only the SJS entry gets found with no indiciation of there being 2 SNSP entries, so possibly some more sophisticated probing and aggregating of results would be better
//         std::cout << "first try nothing found, appending 1 to the query and re-running...\n";
//         uri_encoded_query.append("1");
//         gorazd_query_url = "http://castor.gorazd.org:8080/gorazd/advanced_search;jsessionid=013373AF9710B0E8728F91826ABBD8DD?queryFields=%7B%221%22%3A%7B%22fieldName%22%3A%22HeaderAll%22%2C%22rawFieldQuery%22%3A%22" + uri_encoded_query + "%22%2C%22logTerm%22%3A%22%22%7D%7D";

//         query.fetch(gorazd_query_url);
        
//         if(query.m_get_html.find("\"found\":0,") == std::string::npos) {
//             plain_query.append("1");
//         }
//     }
//     if(query.error_state) query.m_get_html = "\"" + query.m_get_html + "\""; //normally GORAZD returns alread-escaped JSON, but my error-messages are raw strings, and the client expects a JSON response so I have to return a JSON object, which means the raw-string error-messages need to be quoted

//     return "{\"query_form\":\"" + plain_query + "\",\"curl_return_text\":" + query.m_get_html  +"}";
// };

std::string OcsServer::curlGorazd(std::string plain_query, std::string uri_encoded_query) {
    std::string gorazd_query_url_basic = "http://castor.gorazd.org:8080/gorazd/advanced_search;jsessionid=013373AF9710B0E8728F91826ABBD8DD?queryFields=%7B%221%22%3A%7B%22fieldName%22%3A%22HeaderAll%22%2C%22rawFieldQuery%22%3A%22" + uri_encoded_query + "%22%2C%22logTerm%22%3A%22%22%7D%7D";
    
    std::string gorazd_query_url_numbered =  "http://castor.gorazd.org:8080/gorazd/advanced_search;jsessionid=013373AF9710B0E8728F91826ABBD8DD?queryFields=%7B%221%22%3A%7B%22fieldName%22%3A%22HeaderAll%22%2C%22rawFieldQuery%22%3A%22" + uri_encoded_query + "1" + "%22%2C%22logTerm%22%3A%22%22%7D%7D";
    
    std::string gorazd_query_url_reflexive = "http://castor.gorazd.org:8080/gorazd/advanced_search;jsessionid=013373AF9710B0E8728F91826ABBD8DD?queryFields=%7B%221%22%3A%7B%22fieldName%22%3A%22HeaderAll%22%2C%22rawFieldQuery%22%3A%22" + uri_encoded_query + "%C2%A0%D1%81%D1%A7" + "%22%2C%22logTerm%22%3A%22%22%7D%7D";

    std::cout << gorazd_query_url_reflexive << "\n";
    
    CurlFetcher basic_query(gorazd_query_url_basic.c_str());
    CurlFetcher numbered_query(gorazd_query_url_numbered.c_str());
    CurlFetcher reflexive_query(gorazd_query_url_reflexive.c_str());

    bool basic_query_worked = false;
    bool numbered_query_worked = false;
    bool reflexive_query_worked = false;
 
 

    std::thread basic_thread([&]() {
        basic_query.fetch();
        if(basic_query.m_get_html.find("\"found\":0,") == std::string::npos) {
            basic_query_worked = true;
        }
        std::cout << "basic fetch completed, worked?: " << basic_query_worked << "\n";
    });
    std::thread numbered_thread([&]() {
        numbered_query.fetch();
        if(numbered_query.m_get_html.find("\"found\":0,") == std::string::npos) {
            numbered_query_worked = true;
        }
        std::cout << "numbered fetch completed, worked?: " << numbered_query_worked << "\n";
    });
    std::thread reflexive_thread([&]() {
        reflexive_query.fetch();
        if(reflexive_query.m_get_html.find("\"found\":0,") == std::string::npos) {
            reflexive_query_worked = true;
        }
        std::cout << "reflexive fetch completed, worked?: " << reflexive_query_worked << "\n";
    });

    basic_thread.join();
    numbered_thread.join();
    reflexive_thread.join();

    if(basic_query.error_state && numbered_query.error_state && reflexive_query.error_state) {
        return "{\"query_form\":\"" + plain_query + "\",\"curl_return_text\":" + "\"" + basic_query.m_get_html + "\""  +"}";//normally GORAZD returns alread-escaped JSON, but my error-messages are raw strings, and the client expects a JSON response so I have to return a well-formed JSON object, which means the raw-string error-messages need to be quoted
    }

    else if(basic_query_worked) {
        return "{\"query_form\":\"" + plain_query + "\",\"curl_return_text\":" + basic_query.m_get_html  +"}";
    }
    else if(numbered_query_worked) {
        plain_query.append("1");
        return "{\"query_form\":\"" + plain_query + "\",\"curl_return_text\":" + numbered_query.m_get_html  +"}";
    }
    else if(reflexive_query_worked) {
        plain_query.append("\xC2\xA0сѧ");
        return "{\"query_form\":\"" + plain_query + "\",\"curl_return_text\":" + reflexive_query.m_get_html  +"}";
    }

    else return "{\"query_form\":\"" + plain_query + "\",\"curl_return_text\":" + basic_query.m_get_html  +"}"; //not necessary since returning any empty-result query would have the same effect on the front-end and cause the "No results" message.
};
