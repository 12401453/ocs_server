//compile with
//  g++ TcpListener.cpp OcsServer.cpp main.cpp -lsqlite3 -licuuc -licudata -licui18n -lcurl -o webserv
//run with
//  ./webserv <portnumber> [silent]
#include <sstream>
#include "OcsServer.h"
#include <math.h>
#include <sys/stat.h>
#include <vector>

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
        std::cout << std::endl;
        msg_url[lb_pos - 13] = '\0';

        short int page_type = 0;
        if(!strcmp(msg_url, "/text_viewer")) page_type = 1;
        else if(!strcmp(msg_url, "/add_texts")) page_type = 2;
        else if(!strcmp(msg_url, "/words")) page_type = 3;

        //#include "docRoot.cpp"
   
        int url_size = strlen("HTML_DOCS") + sizeof(msg_url); //sizeof() can be used for c-strings declared as an array of char's but strlen() must be used for char *pointers
        char url_c_str[url_size];
    
        strcpy(url_c_str, "HTML_DOCS");
        strcat(url_c_str, msg_url);

        bool cookies_present = false;
        if(page_type > 0) cookies_present = readCookie(m_cookies, msg); //want to avoid reading cookies when serving auxilliary files like stylesheets
        std::cout << "Cookies present? " << cookies_present << std::endl;
        if(cookies_present) {
            std::cout << "text_id cookie: " << m_cookies[0] << "; current_pageno cookie: " << m_cookies[1] << "; subtitle_id cookie: " << m_cookies[2]  << std::endl;
        }

        std::string content = "<h1>404 Not Found</h1>"; 

        std::string content_type = "text/html;charset=UTF-8";
        
        char fil_ext[5];
        for(int i = 0; i < 4; i++) {
            fil_ext[i] = url_c_str[url_size - 5 + i];
        }
        fil_ext[4] = '\0';
        std::cout << "fil_ext = " << fil_ext << std::endl;

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


        buildGETContent(page_type, url_c_str, content, cookies_present);       

     

        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n";
    //    oss << "Cache-Control: no-cache, private \r\n";
        oss << "Content-Type: " << content_type << "\r\n";
        oss << "Content-Length: " << content.size() << "\r\n";
        setCookies(oss);
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


bool OcsServer::c_strStartsWith(const char *str1, const char *str2)
{
    int strcmp_count = 0;
    int str2_len = strlen(str2);
 
    int i = -1;
   
    while ((*str1 == *str2 || *str2 == '\0') && i < str2_len)
    {
        strcmp_count++;
        str1++;
        str2++;
        i++;
    }
 
    if (strcmp_count == str2_len + 1)
    {
        return true;
    }
    else
        return false;
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

 void OcsServer::buildGETContent(short int page_type, char* url_c_str, std::string &content, bool cookies_present) {
    
    std::ifstream urlFile;
    urlFile.open(url_c_str);
    
    if (urlFile.good())
    {
        std::cout << "This file exists and was opened successfully." << std::endl;

        std::ostringstream ss_text;
        std::string line;
        while (std::getline(urlFile, line))
        {   
            if(page_type > 0 && line.find("<?php") != -1) insertTextSelect(ss_text);
            else if(page_type == 1 /*&& cookies_present */ && line.find("<?cky") != -1) void_retrieveText(ss_text); 
            else if(page_type == 1 && line.find("<?cky") != -1) ss_text << "<br><br><div id=\"textbody\"></div>\n";
            
            //For some reason I do not understand, if you insert a <script> tag to declare the below JS variable outside of the script tag at the bottom, on Chrome Android the server very often will get stuck for absolutely ages on loading the Bookerly font file when you refresh the page, and the javascript engine will freeze for about 5-10seconds, but this never happens on Desktop. Thus I've had to make up another bullshit <?js tag to signal where to insert this C++-generated JS
            else if(page_type == 1 /*&& cookies_present*/ && line.find("<?js") != -1) { //the insertTextSelect() function should set all the cookie-values if they are absent, and I need this to run when there are no cookies inorder to set the page_toknos_arr javascript-array to the correct value or functions will break
                ss_text << "let cookie_textselect = " << m_cookies[0] << ";\n";
                ss_text << "page_toknos_arr = " << m_page_toknos_arr << ";\n";
                ss_text << "if(page_toknos_arr.length > 1) {document.getElementById('pagenos').addEventListener('click', selectText_splitup);\ndocument.getElementById('pagenos').addEventListener('keydown', selectText_splitup);}\n";
                //ss_text << "dt_end = " << m_dt_end << ";\n";
                ss_text << "current_pageno = " << m_cookies[1] << ";\n";
            }
            else if(page_type == 1 && line.find("<?js") != -1) ss_text << "let cookie_textselect = 0;\n";

            else if(page_type > 1 && line.find("<?lng") != -1) insertLangSelect(ss_text);

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


void OcsServer::insertTextSelect(std::ostringstream &html) {
    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open("chu.db", &DB)) {
        int prep_code, run_code;
        const char *sql_text;

        sql_text = "SELECT text_id, text_title, tokno_start, tokno_end FROM texts";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);

        int text_id = 0;
        int cookie_text_id = safeStrToInt(m_cookies[0]);
        bool cookie_text_id_exists = false;
        int cookie_subtitle_id = safeStrToInt(m_cookies[2]);
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
            m_cookies[0] = "1";
            m_cookies[1] = "1";
            cookie_text_id = 1;
        }
        else {
            m_cookies[0] = std::to_string(cookie_text_id);
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
            m_cookies[2] = std::to_string(first_subtitle_id);
            m_cookies[1] = "1";
        }
        else {
            m_cookies[2] = std::to_string(cookie_subtitle_id);
        }
        html << "</select>";
        sqlite3_close(DB); 
    }
}

//should never need this shit, get rid
void OcsServer::insertLangSelect(std::ostringstream &html) {
    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open(m_DB_path, &DB)) {
        int prep_code, run_code;
        const char *sql_text;

        sql_text = "SELECT lang_id, lang_name FROM languages ORDER BY lang_name";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);

        int lang_id = 0;
        const char* lang_name;

        while(sqlite3_step(statement) == SQLITE_ROW) {
            lang_id = sqlite3_column_int(statement, 0);
            lang_name = (const char*)sqlite3_column_text(statement, 1);

            html << "<option value=\"" << lang_id << "\"";
            if(lang_id == safeStrToInt(m_cookies[2])) html << " selected";
            html << ">" << lang_name << "</option>\n";
        }

        sqlite3_finalize(statement);
    
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

    if(post_fields == 0 && !strcmp(m_url, "/clear_table.php")) { //clearing the table could be ruinous so adding the extra condition is prudent and should never be run except if post_fields == 0
        std::cout << "m_url: " << m_url << std::endl;
        bool php_func_success = clearTable(clientSocket);
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

    if(!strcmp(m_url, "/get_lang_id.php")) {
        bool php_func_success = getLangId(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/retrieve_subtitle.php")) {
        bool php_func_success = retrieveTextSubtitle(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/retrieve_engword.php")) {
        bool php_func_success = retrieveEngword(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/retrieve_meanings.php")) {
        bool php_func_success = retrieveMeanings(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/lemma_tooltip.php")) {
        bool php_func_success = lemmaTooltips(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/lcs_search.php")) {
        bool php_func_success = lcsSearch(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/lemma_tooltip_mw.php")) {
        bool php_func_success = lemmaTooltipsMW(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/lemma_delete.php")) {
        bool php_func_success = deleteLemma(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/lemma_record.php")) {
        bool php_func_success = recordLemma(post_values, clientSocket);
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
    else if(!strcmp(m_url, "/delete_text.php")) {
        bool php_func_success = deleteText(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/update_db.php")) {
        bool php_func_success = addText(post_values, clientSocket);  
    }
    else if(!strcmp(m_url, "/pull_lemma.php")) {
        bool php_func_success = pullInLemma(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/retrieve_multiword.php")) {
        bool php_func_success = retrieveMultiword(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/retrieve_MW_meanings.php")) {
        bool php_func_success = retrieveMultiwordMeanings(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/record_multiword.php")) {
        bool php_func_success = recordMultiword(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/delete_multiword.php")) {
        bool php_func_success = deleteMultiword(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/pull_multiword.php")) {
        bool php_func_success = pullInMultiword(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/pull_mw_by_form.php")) {
        bool php_func_success = pullInMultiwordByForm(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/update_MW_translations.php")) {
        bool php_func_success = updateMultiwordTranslations(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/curl_lookup.php")) {
        bool php_func_success = curlLookup(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/disregard_word.php")) {
        bool php_func_success = disregardWord(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/dump_lemmas.php")) {
        bool php_func_success = dumpLemmaTable(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/add_text_OE.php")) {
        bool php_func_success = addTextOldEnglish(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/update_displayWord.php")) {
        bool php_func_success = updateDisplayWord(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/lcs_regex_search.php")) {
        bool php_func_success = lcsRegexSearch(post_values, clientSocket);
    }
    else if(!strcmp(m_url, "/greek_tooltips.php")) {
        bool php_func_success = greekTooltips(post_values, clientSocket);
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
    else if(!strcmp(url, "/retrieve_text.php")) return 1;
    else if(!strcmp(url, "/retrieve_text_pageno.php")) return 2;
    else if (!strcmp(url, "/retrieve_text_search.php")) return 1;
    else if(!strcmp(url, "/get_lang_id.php")) return 1;
    else if(!strcmp(url, "/pull_lemma.php")) return 4;
    else if(!strcmp(url, "/lemma_record.php")) return 8;
    else if(!strcmp(url, "/lemma_delete.php")) return 3;
    else if(!strcmp(url, "/retrieve_engword.php")) return 3;
    else if(!strcmp(url, "/retrieve_meanings.php")) return 2;
    else if(!strcmp(url, "/delete_text.php")) return 1;
    else if(!strcmp(url, "/retrieve_multiword.php")) return 3;
    else if(!strcmp(url, "/retrieve_MW_meanings.php")) return 2;
    else if(!strcmp(url, "/record_multiword.php")) return 8;
    else if(!strcmp(url, "/delete_multiword.php")) return 4;
    else if(!strcmp(url, "/update_MW_translations.php")) return 3;
    else if(!strcmp(url, "/pull_multiword.php")) return 2;
    else if(!strcmp(url, "/pull_mw_by_form.php")) return 4;
    else if(!strcmp(url, "/curl_lookup.php")) return 1;
    else if(!strcmp(url, "/disregard_word.php")) return 2;
    else if(!strcmp(url, "/clear_table.php")) return 0;
    else if(!strcmp(url, "/dump_lemmas.php")) return 1;
    else if(!strcmp(url, "/add_text_OE.php")) return 3;
    else if(!strcmp(url, "/update_displayWord.php")) return 5;
    else if(!strcmp(url, "/lemma_tooltip_mw.php")) return 3;
    else if(!strcmp(url, "/lcs_regex_search.php")) return 3;
    else if(!strcmp(url, "/greek_tooltips.php")) return 2;
    else return -1;
}

void OcsServer::setCookies(std::ostringstream &http_response_ss) {
    for(int i = 0; i < 3; i++) {
        http_response_ss << "Set-Cookie: " << m_cookie_names[i] << "=" << m_cookies[i] << "; Max-Age=157680000\r\n";
    }
} 

bool OcsServer::readCookie(std::string cookie[3], const char* msg) {
    int cookie_start = c_strFind(msg, "\r\nCookie") + 9;
    if(cookie_start == 8) {
        m_cookies[0] = "1";
        m_cookies[1] = "1";
        m_cookies[2] = "1";
        return false; //c_strFind() returns -1 if unsuccessful, but I've just added 9 to it so the number signalling no cookies is 8
    }
    const char* cookie_keys[3] {" text_id=", " current_pageno=", " subtitle_id="};

    for(int i = 0; i < 3; i++) {
        int cookieName_start = c_strFind(msg+cookie_start, cookie_keys[i]);
        if(i == 0 && cookieName_start == -1) return false;
        if(cookieName_start == -1) {
            cookie[i] = "1";
            break;
        }
        //printf("cookieName_start: %i\n", cookieName_start);

        int cookieName_length = strlen(cookie_keys[i]);

        int val_length = c_strFindNearest(msg+cookie_start+cookieName_start + cookieName_length, ";", "\r\n");
        //std::cout << "Cookie val_length: " << val_length << "\n";

        char val[val_length + 1];
        for(int j = 0; j < val_length; j++) {
            val[j] = (msg + cookie_start + cookieName_start + cookieName_length)[j];
            //printf("val[j] = %c\n", val[j]);
        }
        val[val_length] = '\0';

        cookie[i] = val;
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

bool OcsServer::addText(std::string _POST[3], int clientSocket) {
    std::cout << "update_db.php" << std::endl;
   
    UErrorCode status = U_ZERO_ERROR;
    std::string text_body = URIDecode(_POST[0]);

    sqlite3 *DB;
    sqlite3_stmt *statement;

    if(!sqlite3_open(m_DB_path, &DB)) {

        icu::BreakIterator *bi = icu::BreakIterator::createWordInstance(icu::Locale::getUS(), status);
        icu::UnicodeString regexp = "\\p{L}"; // from the ICU regex documentation: \p{UNICODE PROPERTY NAME}	Match any character having the specified Unicode Property. (L = letter). This will catch exactly the same things as what the BreakIterator views as word-boundaries, meaning words with thigns like the OCS abbreviation tilde which would get classified as non-words if we used \P{UNICODE PROPERTY NAME} (match characters NOT having the specified property). If the BreakIterator fucks up then this will fuck up, but in that case we were fucked anyway. possibly \w (match Unicode 'words') is better but that also matches numerals which I'm ambivalent about given there can be infinity numbers
        icu::RegexMatcher *matcher = new icu::RegexMatcher(regexp, 0, status); //call delete on this
        
        std::istringstream iss(text_body);

        int prep_code, run_code;
        std::string sql_text_str;
        const char *sql_text;

        int lang_id = std::stoi(_POST[2]);

        sql_text = "SELECT MAX(tokno) FROM display_text";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        run_code = sqlite3_step(statement);
             
        sqlite3_int64 dt_start = sqlite3_column_int64(statement, 0) + 1;
        sqlite3_finalize(statement);

        const char *sql_BEGIN = "BEGIN IMMEDIATE";
        const char *sql_COMMIT = "COMMIT";
              
        prep_code = sqlite3_prepare_v2(DB, sql_BEGIN, -1, &statement, NULL);
        run_code = sqlite3_step(statement);
        sqlite3_finalize(statement);

        for(std::string token; std::getline(iss, token, ' '); ) {
            icu::UnicodeString token_unicode = token.c_str();

            token_unicode.findAndReplace("'", "¬"); //the reason why even normal apostrophes need to be replaced is because the BreakIterator doesn't see them as word-boundaries, so it fucks up; it has nothing to do with SQLite using apostrophes as the string-marker. We restore the apostrophes before inserting them into the table, so we can get rid of icu::UnicodeString::findAndReplace() calls on the retrieveText() etc. functions

            bi->setText(token_unicode);

            int32_t start_range, end_range;
            icu::UnicodeString tb_copy;
            std::string word_eng_word;

            std::string text_word;

            int32_t p = bi->first();
            bool is_a_word = true;
            
            std::string punctuation = "";
            bool punct_empty = true;
            int newline_code = 1;
            bool word_punct = false;

            auto insertWord = [&]() {
                sqlite3_prepare_v2(DB, "INSERT OR IGNORE INTO word_engine (word, lang_id) VALUES (?, ?)", -1, &statement, NULL);
                sqlite3_bind_text(statement, 1, word_eng_word.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(statement, 2, lang_id);
                sqlite3_step(statement);
                sqlite3_finalize(statement);

                sqlite3_prepare_v2(DB, "INSERT INTO display_text (word_engine_id, text_word) SELECT word_engine_id, ? FROM word_engine WHERE word = ? AND lang_id = ?", -1, &statement, NULL);
                sqlite3_bind_text(statement, 1, text_word.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(statement, 2, word_eng_word.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(statement, 3, lang_id);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
            };

            auto insertPunct = [&]() {
                sqlite3_prepare_v2(DB, "INSERT INTO display_text (text_word) VALUES (?)", -1, &statement, NULL);
                sqlite3_bind_text(statement, 1, punctuation.c_str(), -1, SQLITE_STATIC);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
                punctuation = "";
                punct_empty = true;
            };
            auto updateNewline = [&]() {
                sqlite3_prepare_v2(DB, "UPDATE display_text SET space = ? WHERE tokno = last_insert_rowid()", -1, &statement, NULL);
                sqlite3_bind_int(statement, 1, newline_code);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
                newline_code = 1;
            };

            while(p != icu::BreakIterator::DONE) {
                tb_copy = token_unicode;
                start_range = p;
                p = bi->next();
                end_range = p;
                if(end_range == -1) break;
                tb_copy.retainBetween(start_range, end_range);
                
                matcher->reset(tb_copy); //tells the RegexMatcher object to use tb_copy as its haystack
                if(matcher->find()) {
                    is_a_word = true;
                }
                else {
                    is_a_word = false;
                }
                
                if(is_a_word) {
                    if(punct_empty == false) {
                        insertPunct();
                    }
                    if(newline_code > 1) {
                        updateNewline();
                    }

                    tb_copy.toUTF8String(text_word);
                    if(lang_id == 7) {
                        tb_copy.toLower(icu::Locale("tr", "TR"));
                    }
                    else if(lang_id == 12) {
                        tb_copy.toLower(icu::Locale("az", "AZ"));
                    }
                    else{
                        tb_copy.toLower();
                    }
                    
                    tb_copy.toUTF8String(word_eng_word);

                    insertWord();
                }

                else {
                    tb_copy.toUTF8String(text_word);
                    if(text_word == "¬") text_word = "\'";

                    if(text_word == "\n") {
                        if(punct_empty == false) {
                            insertPunct();
                        }
                        newline_code++;
                    }
                    else {
                        if(newline_code > 1) {
                            updateNewline();
                        }
                        punctuation.append(text_word);
                        punct_empty = false;
                    }                
                }
                word_eng_word = ""; //toUTF8String appends the string in tb_copy to word_eng_word rather than overwriting it
                text_word = "";
            }

            if(punct_empty == false) {
                insertPunct();
            }

            if(newline_code > 1) {
                updateNewline();
            }

            sql_text_str = "UPDATE display_text SET space = 1 WHERE tokno = last_insert_rowid() AND space IS NULL";
            //  std::cout << sql_text_str << std::endl;
            sql_text = sql_text_str.c_str();
            prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            run_code = sqlite3_step(statement);
            sqlite3_finalize(statement);
        }

        delete bi;
        delete matcher;
        
        icu::UnicodeString unicode_text_title = URIDecode(_POST[1]).c_str();
        std::string text_title;
       /* unicode_text_title.findAndReplace("’", "\'"); */
        unicode_text_title.toUTF8String(text_title);
        const char* text_title_c_str = text_title.c_str();
       

        sql_text = "INSERT INTO texts (text_title, dt_start, dt_end, lang_id) VALUES (?, ?, last_insert_rowid(), ?)";
        std::cout << "dt_start: " << dt_start << ", text_title: " << text_title << ", last_insert_row_id: " << sqlite3_last_insert_rowid(DB) << std::endl;

        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int64(statement, 2, dt_start);
        sqlite3_bind_text(statement, 1, text_title_c_str, -1, SQLITE_STATIC);
        sqlite3_bind_int(statement, 3, lang_id);

        run_code = sqlite3_step(statement);
        std::cout << "Finalize code: " << sqlite3_finalize(statement) << std::endl;
        std::cout << prep_code << " " << run_code << std::endl;

        sql_text = "SELECT text_id FROM texts WHERE text_id = last_insert_rowid()";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_step(statement);
        int new_text_id = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);
      
        prep_code = sqlite3_prepare_v2(DB, sql_COMMIT, -1, &statement, NULL);
        run_code = sqlite3_step(statement);
        std::cout << prep_code << " " << run_code << std::endl;

        sqlite3_finalize(statement);

        std::cout << "sqlite_close: " << sqlite3_close(DB) << std::endl;

        std::string POST_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\nSet-Cookie: text_id="+std::to_string(new_text_id)+"; Max-Age=157680000\r\nSet-Cookie: lang_id="+std::to_string(lang_id)+"; Max-Age=157680000\r\nSet-Cookie: current_pageno=1; Max-Age=157680000\r\n\r\n";
        int size = POST_response.size() + 1;

        sendToClient(clientSocket, POST_response.c_str(), size);
        return true;
    }
    else {
        std::cout << "Database connection failed on addText()\n";
        return false;
    }   
}

bool OcsServer::deleteText(std::string _POST[1], int clientSocket) {
    int text_id = std::stoi(_POST[0]);
    
    UErrorCode status = U_ZERO_ERROR;
    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open(m_DB_path, &DB)) {
        int prep_code, run_code;
        const char* sql_text;
        
        sql_text = "SELECT MAX(text_id) FROM texts";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        run_code = sqlite3_step(statement);
        int end_text_id = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);

        bool end_text = (end_text_id == text_id);

        sql_text = "SELECT dt_start, dt_end FROM texts WHERE text_id = ?";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, text_id);
        run_code = sqlite3_step(statement);
        std::cout << prep_code << " " << run_code << std::endl;

        sqlite3_int64 dt_start = sqlite3_column_int64(statement, 0);
        sqlite3_int64 dt_end = sqlite3_column_int64(statement, 1);
        sqlite3_finalize(statement);

        sql_text = "DELETE FROM display_text WHERE tokno >= ? AND tokno <= ?";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int64(statement, 1, dt_start);
        sqlite3_bind_int64(statement, 2, dt_end);
        run_code = sqlite3_step(statement);
        std::cout << prep_code << " " << run_code << std::endl;
        sqlite3_finalize(statement);

        sql_text = "DELETE FROM texts WHERE text_id = ?";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, text_id);
        run_code = sqlite3_step(statement);
        std::cout << prep_code << " " << run_code << std::endl;
        sqlite3_finalize(statement);

        sqlite3_close(DB);
    
        std::cout << dt_start << "-->" << dt_end << std::endl;

        std::string POST_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\nSet-Cookie: text_id=" + _POST[0] + "; Max-Age=0\r\nSet-Cookie: current_pageno=1\r\n\r\n";
        int size = POST_response.size() + 1;

        sendToClient(clientSocket, POST_response.c_str(), size);
        return true;
    }
    else {
        std::cout << "Database connection failed on deleteText()\n";
        return false;    
    }  
}

bool OcsServer::lemmaTooltips(std::string _POST[1], int clientSocket) {
    sqlite3* DB;    
    
    if(!sqlite3_open("chu.db", &DB)) {

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
    
    if(!sqlite3_open("chu.db", &DB)) {

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
            lcs_results << "\"" << htmlspecialchars(lcs_result) << "\"" << ",";
            tokno_results << tokno << ",";

            std::ostringstream text_content_result_oss;
            text_content_result_oss << "\"";

            sqlite3_bind_int(statement2, 1, tokno + 5);
            sqlite3_bind_int(statement2, 2, tokno - 5);
            sqlite3_bind_int(statement2, 3, sentence_no);
            while(sqlite3_step(statement2) == SQLITE_ROW) {
                text_content_result_oss << (const char*)sqlite3_column_text(statement2, 2);
                if(sqlite3_column_int(statement2, 0) == tokno) {
                    text_content_result_oss << escapeQuotes("<span class=\"result_word\">") + htmlspecialchars((const char*)sqlite3_column_text(statement2, 1)) + "</span>";
                }
                else text_content_result_oss << htmlspecialchars((const char*)sqlite3_column_text(statement2, 1));
                
                text_content_result_oss << (const char*)sqlite3_column_text(statement2, 3);
            }
            text_content_result_oss << "\"";
            text_results << text_content_result_oss.str() << ",";


            sqlite3_reset(statement2);
            sqlite3_clear_bindings(statement2);
            results_count++;
        }
                
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
    const char* sql_text = "SELECT chu_word_torot, presentation_before, presentation_after, sentence_no, lemma_id, morph_tag, autoreconstructed_lcs, inflexion_class_id, tokno, auto_tagged FROM corpus WHERE tokno >= ? AND tokno <= ?";
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
    int inflexion_class_id = 0;
    int tokno = 0;
    int auto_tagged = 0;
    while(sqlite3_step(statement) == SQLITE_ROW) {
        chu_word_torot = (const char*)sqlite3_column_text(statement, 0);
        presentation_before = (const char*)sqlite3_column_text(statement, 1);
        presentation_after = (const char*)sqlite3_column_text(statement, 2);
        sentence_no_current = sqlite3_column_int(statement, 3);
        lemma_id = sqlite3_column_int(statement, 4);
        inflexion_class_id = sqlite3_column_int(statement, 7);
        tokno = sqlite3_column_int(statement, 8);
        auto_tagged = sqlite3_column_int(statement, 9);
        if(sqlite3_column_type(statement, 5) == SQLITE_NULL) morph_tag = "";
        else morph_tag = (const char*)sqlite3_column_text(statement, 5);
        if(sqlite3_column_type(statement, 6) == SQLITE_NULL) autoreconstructed_lcs = "";
        else autoreconstructed_lcs = (const char*)sqlite3_column_text(statement, 6);

        if(sentence_number_previous > 0 && sentence_no_current != sentence_number_previous) {
            html << "  |  ";
        }

        html << "<span class=\"chunk\">" << applySuperscripts(presentation_before) << "<span class=\"tooltip\" data-tokno=\"" << tokno << "\" data-sentence_no=\"" << sentence_no_current << "\" data-auto_tagged=\"" << auto_tagged << "\""; 
        if(lemma_id > 0) {
            html << " data-lemma_id=\"" << lemma_id << "\"" " data-morph_tag=\"" << morph_tag << "\"";
        }
        if(!autoreconstructed_lcs.empty()) {
            html << " data-lcs_recon=\"" << autoreconstructed_lcs << "\" data-inflexion=\"" << inflexion_class_id << "\"";
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

    if(!sqlite3_open("chu.db", &DB)) {

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
            sentence_count++;

            if(sentence_count % sentences_per_page == 0) {
                page_toknos_arr << "," << sentence_start_tokno - 1 << "],[" << sentence_start_tokno;
                if(pageno_count == 1) first_page_toknos[1] = sentence_start_tokno;
                pageno_count++;
            }
        }
        if(pageno_count == 1) first_page_toknos[1] = first_tokno_end + 1;
        page_toknos_arr << "," << first_tokno_end << "]]";
        m_page_toknos_arr = page_toknos_arr.str();
        m_pageno_count = pageno_count;
        sqlite3_finalize(statement);

        m_cookies[1] = "1";

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
        std::cout << m_page_toknos_arr << "\n";

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

    if(!sqlite3_open("chu.db", &DB)) {

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
            sentence_count++;
            float quotient = trunc(sentence_count / (float)sentences_per_page);
            int remainder = (int)((sentence_count - (float)sentences_per_page*quotient));
            if(remainder == 0) {
                page_toknos_arr << "," << sentence_start_tokno - 1 << "],[" << sentence_start_tokno;
                if(pageno_count == 1) first_page_toknos[1] = sentence_start_tokno;
                pageno_count++;
            }
        }
        if(pageno_count == 1) first_page_toknos[1] = tokno_end + 1;
        page_toknos_arr << "," << tokno_end << "]]";
        m_page_toknos_arr = page_toknos_arr.str();
        m_pageno_count = pageno_count;
        sqlite3_finalize(statement);

        std::ostringstream html;
        html << "<br><div id=\"textbody\">";
        writeTextBody(html, DB, first_page_toknos[0], first_page_toknos[1] - 1);
        html << "</div>";

        // sql_text = "SELECT chu_word_torot, presentation_before, presentation_after, sentence_no FROM corpus WHERE tokno >= ? AND tokno <= ?";
        // sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        // sqlite3_bind_int(statement, 1, first_page_toknos[0]);
        // sqlite3_bind_int(statement, 2, first_page_toknos[1] - 1);
        // const char* chu_word_torot;
        // const char* presentation_before;
        // const char* presentation_after;
        // int sentence_number_previous = 0;
        // int sentence_no_current = 0;
        // html << "<br><div id=\"textbody\">";
        // while(sqlite3_step(statement) == SQLITE_ROW) {
        //     chu_word_torot = (const char*)sqlite3_column_text(statement, 0);
        //     presentation_before = (const char*)sqlite3_column_text(statement, 1);
        //     presentation_after = (const char*)sqlite3_column_text(statement, 2);
        //     sentence_no_current = sqlite3_column_int(statement, 3);

        //     if(sentence_number_previous > 0 && sentence_no_current > sentence_number_previous) {
        //         html << "  |  ";
        //     }

        //     html << "<span class=\"chunk\">" << presentation_before << "<span class=\"tooltip\" data-sentence_no=\"" << sentence_no_current << "\">" << chu_word_torot << "</span>" << presentation_after << "</span>";
        //     sentence_number_previous = sentence_no_current;

        // };
        // sqlite3_finalize(statement);
        // html << "</div>";       

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
        std::cout << m_page_toknos_arr << "\n";

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

    if(!sqlite3_open("chu.db", &DB)) {

        int tokno_start = safeStrToInt(_POST[0]);
        int tokno_end = safeStrToInt(_POST[1]);

        const char *sql_text;

        std::ostringstream html;
        writeTextBody(html, DB, tokno_start, tokno_end);

        // sql_text = "SELECT chu_word_torot, presentation_before, presentation_after, sentence_no FROM corpus WHERE tokno >= ? AND tokno <= ?";
        // sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        // sqlite3_bind_int(statement, 1, first_page_toknos[0]);
        // sqlite3_bind_int(statement, 2, first_page_toknos[1] - 1);
        // const char* chu_word_torot;
        // const char* presentation_before;
        // const char* presentation_after;
        // int sentence_number_previous = 0;
        // int sentence_no_current = 0;
        // html << "<br><div id=\"textbody\">";
        // while(sqlite3_step(statement) == SQLITE_ROW) {
        //     chu_word_torot = (const char*)sqlite3_column_text(statement, 0);
        //     presentation_before = (const char*)sqlite3_column_text(statement, 1);
        //     presentation_after = (const char*)sqlite3_column_text(statement, 2);
        //     sentence_no_current = sqlite3_column_int(statement, 3);

        //     if(sentence_number_previous > 0 && sentence_no_current > sentence_number_previous) {
        //         html << "  |  ";
        //     }

        //     html << "<span class=\"chunk\">" << presentation_before << "<span class=\"tooltip\" data-sentence_no=\"" << sentence_no_current << "\">" << chu_word_torot << "</span>" << presentation_after << "</span>";
        //     sentence_number_previous = sentence_no_current;

        // };
        // sqlite3_finalize(statement);
        // html << "</div>";       

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

bool OcsServer::getLangId(std::string text_id[1], int clientSocket) {

    sqlite3* DB;
    sqlite3_stmt* statement;
  

    if(!sqlite3_open(m_DB_path, &DB)) {
    
        int prep_code, run_code;
        
        std::string sql_text_str = "SELECT lang_id FROM texts WHERE text_id = "+text_id[0];
        const char* sql_text = sql_text_str.c_str();
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        run_code = sqlite3_step(statement);
    
        int lang_id = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);

        int content_length = lang_id > 9 ? 2 : 1;


        std::ostringstream html;
        html << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " << content_length << "\r\nSet-Cookie: lang_id=" << lang_id << "; Max-Age=157680000" << "\r\n" << "Cache-Control: no-cache" << "\r\n\r\n" << lang_id;
        int length = html.str().size() + 1;

        sqlite3_close(DB);

        sendToClient(clientSocket, html.str().c_str(), length);
        
        return true;
    }
    else return false;
}

void OcsServer::void_retrieveText(std::ostringstream &html) {
    //these should all have been corrected to sensible values by the insertTextSelect() function
    int cookie_textselect = safeStrToInt(m_cookies[0]);
    int cookie_pageno = safeStrToInt(m_cookies[1]);
    int cookie_subtitle_id = safeStrToInt(m_cookies[2]);  

    int sentences_per_page = 30;
    std::ostringstream page_toknos_arr;
    page_toknos_arr << "[";
    int pageno_count = 1;
    int sentence_count = 0;

    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open("chu.db", &DB)) {

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
            sentence_count++;

            // std::cout << "sentence_count:" << sentence_count << "; sentences_per_page: " << sentences_per_page << "\n";

            if(sentence_count % sentences_per_page == 0) {
                page_toknos_arr << "," << sentence_start_tokno - 1 << "],[" << sentence_start_tokno;
                page_toknos_vec.push_back(sentence_start_tokno);
                pageno_count++;
            }
        }
        page_toknos_vec.push_back(tokno_end + 1);
        page_toknos_arr << "," << tokno_end << "]]";
        m_page_toknos_arr = page_toknos_arr.str();
        m_pageno_count = pageno_count;
        sqlite3_finalize(statement);

        if(cookie_pageno > pageno_count) {
            cookie_pageno = 1;
        }
        m_cookies[1] = std::to_string(cookie_pageno);

        html << "<br><div id=\"textbody\">";
        writeTextBody(html, DB, page_toknos_vec[cookie_pageno - 1], page_toknos_vec[cookie_pageno] - 1);

        // sql_text = "SELECT chu_word_torot, presentation_before, presentation_after, sentence_no FROM corpus WHERE tokno >= ? AND tokno <= ?"; // OR text_word = '\n')";
        // sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        // sqlite3_bind_int(statement, 1, page_toknos_vec[cookie_pageno - 1]);
        // sqlite3_bind_int(statement, 2, page_toknos_vec[cookie_pageno] - 1);
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

        if(m_pageno_count > 1) {
            html << "<br><br><div id='pageno_footer'><div id=\"pagenos\">";
            html << "<div id=\"pageno_leftarrow\" class=\"nav_arrow\">&lt;</div>";
            html << "<textarea id=\"pageno_box\" spellcheck=\"false\" autocomplete=\"off\">";
            html << m_cookies[1];
            html << "</textarea>";
            html << "<div class=\"pageno_text\" style=\"width: 40px;\">of</div>";
            html << "<div class=\"pageno_text\">" << m_pageno_count << "</div>";
            
            html << "<div id=\"pageno_rightarrow\" class=\"nav_arrow\">&gt;</div>";             
            html << "</div></div>";
        }
        else {
            html << "<br>";
        }
        std::cout << m_page_toknos_arr << "\n";

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

bool OcsServer::retrieveEngword(std::string _POST[3], int clientSocket) {
    
    sqlite3 *DB;
    sqlite3_stmt *statement;

    if(!sqlite3_open(m_DB_path, &DB)) {

        int word_engine_id = std::stoi(_POST[0]);
        sqlite_int64 tokno_current = std::stoi(_POST[1]);
        int lang_id = std::stoi(_POST[2]);

        const char* word_eng_word;
        int first_lemma_id = 0;

        int existing_lemma_id = 0;

        int prep_code, run_code;

        const char* sql_text = "SELECT word, first_lemma_id FROM word_engine WHERE word_engine_id = ?";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, word_engine_id);
        run_code = sqlite3_step(statement);
        
        word_eng_word = (const char*)sqlite3_column_text(statement, 0);
        std::string word_eng_word_str = "";
        if(word_eng_word != nullptr) word_eng_word_str = word_eng_word;

        first_lemma_id = sqlite3_column_int(statement, 1); //will be 0 if the field is empty
        sqlite3_finalize(statement);
        short int lemma_meaning_no = 0;

        sql_text = "SELECT lemma_id, lemma_meaning_no FROM display_text WHERE tokno = ?";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int64(statement, 1, tokno_current);
        run_code = sqlite3_step(statement);
        existing_lemma_id = sqlite3_column_int(statement, 0);
        lemma_meaning_no = sqlite3_column_int(statement, 1);
        sqlite3_finalize(statement);

        std::cout << "word_eng_word = " << word_eng_word_str << "; first_lemma_id = " << first_lemma_id << "; existing_lemma_id = " << existing_lemma_id << std::endl;

        std::string lemma_tag_content = "";
        std::string lemma_textarea_content = "";
        int lemma_id = 0;
     
        short int pos = 1;

        if(!first_lemma_id) {
            lemma_tag_content = word_eng_word_str;

            //could return more than one row but we just take the first as the default
            if(lang_id == 5) sql_text = "SELECT eng_trans1, pos, lemma_id FROM lemmas WHERE lemma LIKE ? AND lang_id = ?"; //case-folding to account for German nouns //this won't work for nouns that start with umlauted letters, do better
            else sql_text = "SELECT eng_trans1, pos, lemma_id FROM lemmas WHERE lemma = ? AND lang_id = ?";
            prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_bind_text(statement, 1, word_eng_word_str.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(statement, 2, lang_id);

            run_code = sqlite3_step(statement);
            const unsigned char* lemma_textarea_content_rawsql = sqlite3_column_text(statement, 0);
            if(lemma_textarea_content_rawsql != nullptr) lemma_textarea_content = (const char*)lemma_textarea_content_rawsql;
            pos = sqlite3_column_int(statement, 1);
            lemma_id = sqlite3_column_int(statement, 2); //will remain 0 if no existing lemma is found
            if(!pos) pos = 1;
            sqlite3_finalize(statement);
        }
        else if(!existing_lemma_id) {
            sql_text = "SELECT lemma, pos, eng_trans1, eng_trans2, eng_trans3, eng_trans4, eng_trans5, eng_trans6, eng_trans7, eng_trans8, eng_trans9, eng_trans10 FROM lemmas WHERE lemma_id = ?";
            prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, first_lemma_id);
            run_code = sqlite3_step(statement);

            const unsigned char* lemma_rawsql = sqlite3_column_text(statement, 0);
            if(lemma_rawsql != nullptr) lemma_tag_content = (const char*)lemma_rawsql;

            pos = sqlite3_column_int(statement, 1);

            //ensure that the first non-empty meaning is returned incase some bastard fills in meaning no.10 first
            const unsigned char* lemma_textarea_content_rawsql;
            for(int i = 2; i < 12; i++) {
                lemma_textarea_content_rawsql = sqlite3_column_text(statement, i);
                if(lemma_textarea_content_rawsql != nullptr) {
                    lemma_textarea_content = (const char*)lemma_textarea_content_rawsql; //this is implicitly a std::string construction from const char*
                    lemma_meaning_no = i - 1;
                    break;
                }
            }
            sqlite3_finalize(statement);
            lemma_id = first_lemma_id;
        }
        else {
            std::string sql_text_str = "SELECT lemma, pos, eng_trans"+std::to_string(lemma_meaning_no)+" FROM lemmas WHERE lemma_id = ?";
            sql_text = sql_text_str.c_str();
            prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, existing_lemma_id);
            run_code = sqlite3_step(statement);

            const unsigned char* lemma_rawsql = sqlite3_column_text(statement, 0);
            if(lemma_rawsql != nullptr) lemma_tag_content = (const char*)lemma_rawsql;
            pos = sqlite3_column_int(statement, 1);

            const unsigned char* lemma_textarea_rawsql = sqlite3_column_text(statement, 2);
            if(lemma_textarea_rawsql != nullptr) lemma_textarea_content = (const char*)lemma_textarea_rawsql;

            sqlite3_finalize(statement);

            lemma_id = existing_lemma_id;
        }

        sqlite3_close(DB);


        std::ostringstream json;
        json << "{\"lemma_tag_content\":\"" << escapeQuotes(lemma_tag_content) << "\",\"lemma_textarea_content\":\"" << escapeQuotes(lemma_textarea_content) << "\",\"lemma_meaning_no\":\"" << lemma_meaning_no << "\",\"lemma_id\":\"" << lemma_id << "\",\"pos\":\"" << pos << "\"}";

        int content_length = json.str().size();
        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json.str();
        int length = post_response.str().size() + 1;

        sendToClient(clientSocket, post_response.str().c_str(), length);

        
        return true;
    }
    else {
        std::cout << "Database connection failed on retrieveEngword\n";
        return false;
    }
}

bool OcsServer::recordLemma(std::string _POST[8], int clientSocket) {

    sqlite3* DB;
    sqlite3_stmt* statement;

    if (!sqlite3_open(m_DB_path, &DB)) {
        int word_engine_id = std::stoi(_POST[0]);

        std::string lemma_form = URIDecode(_POST[1]);
        std::string lemma_meaning = URIDecode(_POST[2]);
        short int lemma_meaning_no = safeStrToInt(_POST[3]);
        short int lang_id = std::stoi(_POST[4]);
        sqlite3_int64 tokno = std::stoi(_POST[5]);
        short int pos = std::stoi(_POST[6]);
        short int submitted_lemma_meaning_no = std::stoi(_POST[7]);

        const char* sql_BEGIN = "BEGIN IMMEDIATE";
        const char* sql_COMMIT = "COMMIT";

        sqlite3_prepare_v2(DB, sql_BEGIN, -1, &statement, NULL);
        sqlite3_step(statement);
        sqlite3_finalize(statement);

        //this will get skipped if we are assigning an existing lemma to a new form
        const char* sql_text = "INSERT OR IGNORE INTO lemmas (lemma, lang_id, pos) VALUES (?, ?, ?)";
        int prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_text(statement, 1, lemma_form.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(statement, 2, lang_id);
        sqlite3_bind_int(statement, 3, pos);
        int run_code = sqlite3_step(statement);
        sqlite3_finalize(statement);

        sql_text = "SELECT first_lemma_id FROM word_engine WHERE word_engine_id = ?";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, word_engine_id);
        run_code = sqlite3_step(statement);
        int first_lemma_id = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);

        sql_text = "SELECT lemma_id FROM display_text WHERE tokno = ?";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int64(statement, 1, tokno);
        run_code = sqlite3_step(statement);
        int lemma_id_current = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);

        //get lemma_id of the lemma we just (tried to) insert (it may already have been in the table so we have to get it by its set of unique columns)
        sql_text = "SELECT lemma_id FROM lemmas WHERE lemma = ? AND lang_id = ? AND pos = ?";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_text(statement, 1, lemma_form.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(statement, 2, lang_id);
        sqlite3_bind_int(statement, 3, pos);
        run_code = sqlite3_step(statement);
        int target_lemma_id = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);

        std::string sql_text_str = "";
        if (lemma_meaning == "") {
            sql_text_str = "UPDATE lemmas SET eng_trans" + std::to_string(lemma_meaning_no) + " = NULL WHERE lemma_id = ?";
            sql_text = sql_text_str.c_str();
            prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, target_lemma_id);
        }
        else {
            sql_text_str = "UPDATE lemmas SET eng_trans" + std::to_string(lemma_meaning_no) + " = ? WHERE lemma_id = ?";
            sql_text = sql_text_str.c_str();
            prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_bind_text(statement, 1, lemma_meaning.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(statement, 2, target_lemma_id);
        }
        std::cout << sql_text_str << std::endl;
        std::cout << "target_lemma_id: " << target_lemma_id << std::endl;
        
        run_code = sqlite3_step(statement);
        sqlite3_finalize(statement);

        if (!lemma_id_current) {

            if (lemma_meaning_no == submitted_lemma_meaning_no) {
                sql_text = "UPDATE display_text SET lemma_id = ?, lemma_meaning_no = ? WHERE tokno = ?";
                prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
                sqlite3_bind_int(statement, 1, target_lemma_id);
                sqlite3_bind_int(statement, 2, submitted_lemma_meaning_no);
                sqlite3_bind_int64(statement, 3, tokno);
                run_code = sqlite3_step(statement);
                sqlite3_finalize(statement);
            }
        }
        else {
            if (lemma_id_current != target_lemma_id) {
                if (lemma_meaning_no == submitted_lemma_meaning_no) {
                    sql_text = "UPDATE display_text SET lemma_id = ?, lemma_meaning_no = ? WHERE tokno = ?";
                    prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
                    sqlite3_bind_int(statement, 1, target_lemma_id);
                    sqlite3_bind_int(statement, 2, submitted_lemma_meaning_no);
                    sqlite3_bind_int64(statement, 3, tokno);
                    run_code = sqlite3_step(statement);
                    sqlite3_finalize(statement);
                }
            }
            //if the lemma we are submitting is the same as that already bound to this display_word then we leave the lemma_id unchanged as just re-set the lemma_meaning_no
            else {
                if (lemma_meaning_no == submitted_lemma_meaning_no) {
                    sql_text = "UPDATE display_text SET lemma_meaning_no = ? WHERE tokno = ?";
                    prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
                    sqlite3_bind_int(statement, 1, submitted_lemma_meaning_no);
                    sqlite3_bind_int64(statement, 2, tokno);
                    run_code = sqlite3_step(statement);
                }
            }
        }

        if (!first_lemma_id) {
            sql_text = "UPDATE word_engine SET first_lemma_id = ? WHERE word_engine_id = ?";
            prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, target_lemma_id);
            sqlite3_bind_int(statement, 2, word_engine_id);
            run_code = sqlite3_step(statement);
            sqlite3_finalize(statement);
        }


        sqlite3_prepare_v2(DB, sql_COMMIT, -1, &statement, NULL);
        sqlite3_step(statement);
        sqlite3_finalize(statement);

        sqlite3_close(DB);

        std::string POST_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
        int size = POST_response.size() + 1;

        sendToClient(clientSocket, POST_response.c_str(), size);
        return true;
    }
    else {
        std::cout << "Database connection failed on recordLemma\n";
        return false;
    }
}

bool OcsServer::pullInLemma(std::string _POST[4], int clientSocket) {
    sqlite3* DB;
    sqlite3_stmt* statement;

    if (!sqlite3_open(m_DB_path, &DB)) {

        std::string sql_text_str = "";
        std::string lemma_form = URIDecode(_POST[0]);
        short int lemma_meaning_no = safeStrToInt(_POST[1]);
        short int pos = std::stoi(_POST[2]);
        int lang_id = std::stoi(_POST[3]);

        if (pos == 0) {
            if(lang_id == 5) sql_text_str = "SELECT eng_trans" + std::to_string(lemma_meaning_no) + ", lemma_id, pos FROM lemmas WHERE lemma LIKE ? AND lang_id = ?"; //casefold German to cope with nouns
            else sql_text_str = "SELECT eng_trans" + std::to_string(lemma_meaning_no) + ", lemma_id, pos FROM lemmas WHERE lemma = ? AND lang_id = ?";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_text(statement, 1, lemma_form.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(statement, 2, lang_id);
        }
        else {
            if(lang_id == 5) sql_text_str = "SELECT eng_trans" + std::to_string(lemma_meaning_no) + ", lemma_id FROM lemmas WHERE lemma LIKE ? AND pos = ? AND lang_id = ?"; //casefold German to cope with nouns
            else sql_text_str = "SELECT eng_trans" + std::to_string(lemma_meaning_no) + ", lemma_id FROM lemmas WHERE lemma = ? AND pos = ? AND lang_id = ?";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_text(statement, 1, lemma_form.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(statement, 2, pos);
            sqlite3_bind_int(statement, 3, lang_id);
        }

        std::cout << sql_text_str << std::endl;
                
        sqlite3_step(statement);
        int lemma_id = sqlite3_column_int(statement, 1);
        std::string lemma_textarea_content;
        if (lemma_id) {
            lemma_textarea_content = sqlite3_column_text(statement, 0) == nullptr ? "" : (const char*)sqlite3_column_text(statement, 0);
        }
        if (pos == 0) {
            pos = sqlite3_column_int(statement, 2);
        }

        sqlite3_finalize(statement);

        sqlite3_close(DB);

        std::ostringstream json;

        if (!lemma_id) {
            json << "{\"lemma_textarea_content\":null,\"lemma_id\":0,\"pos\":null}";
        }
        else {
            json << "{\"lemma_textarea_content\":\"" << escapeQuotes(lemma_textarea_content) << "\",\"lemma_id\":\"" << lemma_id << "\",\"pos\":\"" << pos << "\"}";
        }

        int content_length = json.str().size();
        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json.str();
        int length = post_response.str().size() + 1;

        sendToClient(clientSocket, post_response.str().c_str(), length);

        return true;
    }
    else {
        std::cout << "Database connection failed on pullInLemma()" << std::endl;
        return false;
    }

}

bool OcsServer::retrieveMeanings(std::string _POST[2], int clientSocket) {
    
    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open(m_DB_path, &DB)) {

        short int lemma_meaning_no = safeStrToInt(_POST[1]);
        int lemma_id = std::stoi(_POST[0]);

        std::string sql_text_str = "SELECT eng_trans"+std::to_string(lemma_meaning_no)+" FROM lemmas WHERE lemma_id = ?";
        sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, lemma_id);
        sqlite3_step(statement);

        std::string lemma_textarea_content = "";
        const unsigned char* lemma_textarea_content_rawsql = sqlite3_column_text(statement, 0);
        if(lemma_textarea_content_rawsql != nullptr) lemma_textarea_content = (const char*)lemma_textarea_content_rawsql;

        sqlite3_finalize(statement);

        sqlite3_close(DB);

        int content_length = lemma_textarea_content.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " << content_length << "\r\n\r\n" << lemma_textarea_content;

        int length = post_response.str().size() + 1;

        sendToClient(clientSocket, post_response.str().c_str(), length);

        
        return true;
    }
    else {
        std::cout << "Database connection failed on retrieveMeanings()" << std::endl;
        return false;
    }
}

bool OcsServer::retrieveMultiwordMeanings(std::string _POST[2], int clientSocket) {
    
    sqlite3* DB;
    
    if(!sqlite3_open(m_DB_path, &DB)) {
        sqlite3_stmt* statement;
       
        int multiword_id = std::stoi(_POST[0]);
        int mw_lemma_meaning_no = safeStrToInt(_POST[1]);

        std::string sql_text_str = "SELECT eng_trans"+std::to_string(mw_lemma_meaning_no)+" FROM multiword_lemmas WHERE multiword_id = ?";
        sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, multiword_id);
        sqlite3_step(statement);

        std::string lemma_textarea_content = "";
        const unsigned char* lemma_textarea_content_rawsql = sqlite3_column_text(statement, 0);
        if(lemma_textarea_content_rawsql != nullptr) lemma_textarea_content = (const char*)lemma_textarea_content_rawsql;

        sqlite3_finalize(statement);

        sqlite3_close(DB);

        int content_length = lemma_textarea_content.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " << content_length << "\r\n\r\n" << lemma_textarea_content;

        int length = post_response.str().size() + 1;

        sendToClient(clientSocket, post_response.str().c_str(), length);

        
        return true;
    }
    else {
        std::cout << "Database connection failed on retrieveMultiwordMeanings()" << std::endl;
        return false;
    }
}

bool OcsServer::deleteLemma(std::string _POST[3], int clientSocket) {

    sqlite3* DB;

    if(!sqlite3_open(m_DB_path, &DB)) {
        const char *sql_BEGIN = "BEGIN IMMEDIATE";
        const char *sql_COMMIT = "COMMIT";
        sqlite3_stmt* statement;

        int lemma_id = std::stoi(_POST[0]);
        int word_engine_id = std::stoi(_POST[1]);
        sqlite3_int64 tokno_current = std::stol(_POST[2]);

        bool lemma_still_set = true;
        //////////////////////
        sqlite3_exec(DB, sql_BEGIN, nullptr, nullptr, nullptr);
        /////////////////////
        //NON-INDEXED LOOKUP
        std::string sql_text_str = "SELECT lemma_id FROM display_text WHERE word_engine_id = ? AND lemma_id IS NOT NULL AND lemma_id != ?";
        sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, word_engine_id);
        sqlite3_bind_int(statement, 2, lemma_id);
        sqlite3_step(statement);
        int leftover_lemma_id = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);

        //could be less code by just if/elseing the sql_text_str but this way is clearer and not less efficient
        if(!leftover_lemma_id) {
            std::cout << "no leftover_lemma_id" << std::endl;

           /* sqlite3_prepare_v2(DB, sql_BEGIN, -1, &statement, NULL);
            sqlite3_step(statement);
            sqlite3_finalize(statement); */

            sql_text_str = "UPDATE word_engine SET first_lemma_id = NULL WHERE word_engine_id = ?";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, word_engine_id);
            sqlite3_step(statement);
            sqlite3_finalize(statement);
 
            sql_text_str = "UPDATE display_text SET lemma_meaning_no = NULL, lemma_id = NULL WHERE word_engine_id = ?";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, word_engine_id);
            sqlite3_step(statement);
            sqlite3_finalize(statement);

            //can this commit be left till the very end of will it cause the SELECTS to work on old data?
           /* sqlite3_prepare_v2(DB, sql_COMMIT, -1, &statement, NULL);
            sqlite3_step(statement);
            sqlite3_finalize(statement); */

            sql_text_str = "SELECT word_engine_id FROM word_engine WHERE first_lemma_id = ?";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, lemma_id);
            sqlite3_step(statement);
            int other_lemma_form = sqlite3_column_int(statement, 0);
            sqlite3_finalize(statement);

            //NON-INDEXED LOOKUP
            sql_text_str = "SELECT tokno FROM display_text WHERE lemma_id = ? AND tokno != ?";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, lemma_id);
            sqlite3_bind_int64(statement, 2, tokno_current);
            sqlite3_step(statement);
            int other_textword_with_current_lemma = sqlite3_column_int(statement, 0);
            sqlite3_finalize(statement);


            if(!other_lemma_form && !other_textword_with_current_lemma) {
                std::cout << "no other_lemma_form" << std::endl;
                sql_text_str = "DELETE FROM lemmas WHERE lemma_id = ?";
                sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
                sqlite3_bind_int(statement, 1, lemma_id);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
            }
            lemma_still_set = false;
        }
        else {
            /*sqlite3_prepare_v2(DB, sql_BEGIN, -1, &statement, NULL);
            sqlite3_step(statement);
            sqlite3_finalize(statement); */

            sql_text_str = "UPDATE word_engine SET first_lemma_id = ? WHERE word_engine_id = ?";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, leftover_lemma_id);
            sqlite3_bind_int(statement, 2, word_engine_id);
            sqlite3_step(statement);
            sqlite3_finalize(statement);

            sql_text_str = "UPDATE display_text SET lemma_meaning_no = NULL, lemma_id = NULL WHERE lemma_id = ?";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, lemma_id);
            sqlite3_step(statement);
            sqlite3_finalize(statement);

          /*  sqlite3_prepare_v2(DB, sql_COMMIT, -1, &statement, NULL);
            sqlite3_step(statement);
            sqlite3_finalize(statement); */

            sql_text_str = "SELECT word_engine_id FROM word_engine WHERE first_lemma_id = ?";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, lemma_id);
            sqlite3_step(statement);
            int other_lemma_form = sqlite3_column_int(statement, 0);
            sqlite3_finalize(statement);

            //NON-INDEXED LOOKUP
            sql_text_str = "SELECT tokno FROM display_text WHERE lemma_id = ? AND tokno != ?";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, lemma_id);
            sqlite3_bind_int64(statement, 2, tokno_current);
            sqlite3_step(statement);
            int other_textword_with_current_lemma = sqlite3_column_int(statement, 0);
            sqlite3_finalize(statement);

            if(!other_lemma_form && !other_textword_with_current_lemma) {
                std::cout << "no other_lemma_form" << std::endl;
                sql_text_str = "DELETE FROM lemmas WHERE lemma_id = ?";
                sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
                sqlite3_bind_int(statement, 1, lemma_id);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
            }
            lemma_still_set = true;
        }
        //////////////////////////
        sqlite3_exec(DB, sql_COMMIT, nullptr, nullptr, nullptr);
        /////////////////////////
        sqlite3_close(DB);
        
        std::ostringstream POST_response;
        POST_response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 1\r\n\r\n" << lemma_still_set;
        int size = POST_response.str().size() + 1;


        sendToClient(clientSocket, POST_response.str().c_str(), 65);
        
        return true;
    }
    else {
        std::cout << "Database connection failed on deleteLemma()" << std::endl;
        return false;
    }

}

bool OcsServer::clearTable(int clientSocket) {

    sqlite3* DB;

    if(!sqlite3_open(m_DB_path, &DB)) {
        sqlite3_stmt* statement;

        const char *sql_BEGIN = "BEGIN IMMEDIATE";
        const char *sql_COMMIT = "COMMIT";

        sqlite3_prepare_v2(DB, sql_BEGIN, -1, &statement, NULL);
        sqlite3_step(statement);
        sqlite3_finalize(statement);

        const char* sql_text = "DROP TABLE IF EXISTS display_text;CREATE TABLE display_text (tokno INTEGER PRIMARY KEY, text_word TEXT, space INTEGER, word_engine_id INTEGER, lemma_meaning_no INTEGER, lemma_id INTEGER, multiword_id INTEGER, multiword_meaning_no INTEGER, multiword_count INTEGER);CREATE INDEX mw_count_index ON display_text(multiword_count) WHERE multiword_count IS NOT NULL;CREATE INDEX lemma_id_index ON display_text(lemma_id) WHERE lemma_id IS NOT NULL";
        sqlite3_exec(DB, sql_text, nullptr, nullptr, nullptr);

        sql_text = "DROP TABLE IF EXISTS word_engine;CREATE TABLE word_engine (word_engine_id INTEGER PRIMARY KEY, word TEXT, lang_id INTEGER, first_lemma_id INTEGER, UNIQUE(word, lang_id))";
        sqlite3_exec(DB, sql_text, nullptr, nullptr, nullptr);

        sql_text = "DROP TABLE IF EXISTS texts;CREATE TABLE texts (text_id INTEGER PRIMARY KEY, text_title TEXT, dt_start INTEGER, dt_end INTEGER, lang_id INTEGER, text_tag TEXT)";
        sqlite3_exec(DB, sql_text, nullptr, nullptr, nullptr);

        sql_text = "DROP TABLE IF EXISTS lemmas;CREATE TABLE lemmas (lemma_id INTEGER PRIMARY KEY, lemma TEXT, eng_trans1 TEXT, eng_trans2 TEXT, eng_trans3 TEXT, eng_trans4 TEXT, eng_trans5 TEXT, eng_trans6 TEXT, eng_trans7 TEXT, eng_trans8 TEXT, eng_trans9 TEXT, eng_trans10 TEXT, lang_id INTEGER, pos INTEGER, UNIQUE(lemma, lang_id, pos))";
        sqlite3_exec(DB, sql_text, nullptr, nullptr, nullptr);

        sql_text = "DROP TABLE IF EXISTS multiwords;CREATE TABLE multiwords (multiword_id INTEGER, word_eng_id1 INTEGER DEFAULT 0 NOT NULL, word_eng_id2 INTEGER DEFAULT 0 NOT NULL, word_eng_id3 INTEGER DEFAULT 0 NOT NULL, word_eng_id4 INTEGER DEFAULT 0 NOT NULL, word_eng_id5 INTEGER DEFAULT 0 NOT NULL, word_eng_id6 INTEGER DEFAULT 0 NOT NULL, word_eng_id7 INTEGER DEFAULT 0 NOT NULL, word_eng_id8 INTEGER DEFAULT 0 NOT NULL, word_eng_id9 INTEGER DEFAULT 0 NOT NULL, word_eng_id10 INTEGER DEFAULT 0 NOT NULL, lang_id INTEGER, UNIQUE(multiword_id, lang_id, word_eng_id1, word_eng_id2, word_eng_id3, word_eng_id4, word_eng_id5, word_eng_id6, word_eng_id7, word_eng_id8, word_eng_id9, word_eng_id10))";
        sqlite3_exec(DB, sql_text, nullptr, nullptr, nullptr);

        sql_text = "DROP TABLE IF EXISTS multiword_lemmas;CREATE TABLE multiword_lemmas (multiword_id INTEGER PRIMARY KEY, multiword_lemma_form TEXT, eng_trans1 TEXT, eng_trans2 TEXT, eng_trans3 TEXT, eng_trans4 TEXT, eng_trans5 TEXT, pos INTEGER, lang_id INTEGER, UNIQUE(multiword_lemma_form, pos, lang_id))";
        sqlite3_exec(DB, sql_text, nullptr, nullptr, nullptr);

        sql_text = "DROP TABLE IF EXISTS languages;CREATE TABLE languages (lang_id INTEGER PRIMARY KEY, lang_name TEXT UNIQUE)";
        sqlite3_exec(DB, sql_text, nullptr, nullptr, nullptr);

        sql_text = "DROP TABLE IF EXISTS context_trans;CREATE TABLE context_trans (dt_start INTEGER, dt_end INTEGER, eng_trans TEXT, UNIQUE(dt_start, dt_end))";
        sqlite3_exec(DB, sql_text, nullptr, nullptr, nullptr);

        sql_text = "INSERT INTO languages (lang_id, lang_name) VALUES (8, 'Danish'), (7, 'Turkish'), (1, 'Russian'), (2, 'Kazakh'), (3, 'Polish'), (4, 'Bulgarian'), (6, 'Swedish'), (5, 'German'), (10, 'Old English')";
        sqlite3_exec(DB, sql_text, nullptr, nullptr, nullptr);

        sqlite3_prepare_v2(DB, sql_COMMIT, -1, &statement, NULL);
        sqlite3_step(statement);
        sqlite3_finalize(statement);

        sql_text = "VACUUM";
        sqlite3_exec(DB, sql_text, nullptr, nullptr, nullptr);
        

        sqlite3_close(DB);

        std::string POST_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
        int size = POST_response.size() + 1;
        sendToClient(clientSocket, POST_response.c_str(), size);
        return true;
    }
    else {
        std::cout << "Database connection failed on clearTable()" << std::endl;
        return false;
    }

}

bool OcsServer::retrieveMultiword(std::string _POST[3], int clientSocket) {

    sqlite3* DB;
    UErrorCode status = U_ZERO_ERROR;

    if(!sqlite3_open(m_DB_path, &DB)) {
        sqlite3_stmt* statement;

        sqlite3_int64 tokno = std::stol(_POST[1]);
        int word_engine_id = std::stoi(_POST[0]);
        int lang_id = std::stoi(_POST[2]);
        
        short int pos = 1;

        icu::UnicodeString regexp = "\\p{STerm}";
        icu::RegexMatcher matcher(regexp, 0, status);
        
        icu::UnicodeString text_word = "";
        const unsigned char* text_word_rawsql;
        const char* sql_text = "SELECT text_word, tokno, word_engine_id FROM display_text WHERE tokno = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);

        int display_text_word_eng_id = 0;
        std::ostringstream adjacent_toknos_array;
        adjacent_toknos_array << "[";

        for(int i = -1; i > -51; i--) {
           
            text_word = "";
            sqlite3_bind_int64(statement, 1, tokno + i);
            sqlite3_step(statement);

            display_text_word_eng_id = sqlite3_column_int(statement, 2);
            if(display_text_word_eng_id) {
                adjacent_toknos_array << sqlite3_column_int64(statement, 1) << ",";
            }
            else {
                text_word_rawsql = sqlite3_column_text(statement, 0);
                if(text_word_rawsql != nullptr) text_word = (const char*)text_word_rawsql;

                matcher.reset(text_word);
                if(matcher.find()) {
                    sqlite3_reset(statement);
                    break;
                }
                /* this way is probably far quicker and sufficient for most European languages, but the ICU way is more robust
                if(!strcmp(text_word, ".") || !strcmp(text_word, ";") || !strcmp(text_word, "!") || !strcmp(text_word, "?") || !strcmp(text_word, ".")) {
                    break;
                }*/
            }
            sqlite3_reset(statement);
        }
        for(int i = 1; i < 51; i++) {
           
            text_word = "";
            sqlite3_bind_int64(statement, 1, tokno + i);
            sqlite3_step(statement);

            display_text_word_eng_id = sqlite3_column_int(statement, 2);
            if(display_text_word_eng_id) {
                adjacent_toknos_array << sqlite3_column_int64(statement, 1) << ",";
            }
            else {
                text_word_rawsql = sqlite3_column_text(statement, 0);
                if(text_word_rawsql != nullptr) text_word = (const char*)text_word_rawsql;

                matcher.reset(text_word);
                if(matcher.find()) {
                    sqlite3_reset(statement);
                    break;
                }
                /* this way is probably far quicker and sufficient for most European languages, but the ICU way is more robust
                if(!strcmp(text_word, ".") || !strcmp(text_word, ";") || !strcmp(text_word, "!") || !strcmp(text_word, "?") || !strcmp(text_word, ".")) {
                    break;
                }*/
            }
            sqlite3_reset(statement);
        }
        sqlite3_finalize(statement);
        int strm_pos = adjacent_toknos_array.tellp();
        if(strm_pos > 1) adjacent_toknos_array.seekp(strm_pos - 1);
        adjacent_toknos_array << "]";

        sql_text = "SELECT multiword_id, multiword_meaning_no FROM display_text WHERE tokno = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int64(statement, 1, tokno);
        sqlite3_step(statement);

        int multiword_id = 0;
        short int multiword_meaning_no = 1;
        multiword_id = sqlite3_column_int(statement, 0);

        std::string mw_lemma_form = "";
        std::string mw_lemma_meaning = "";
        if(multiword_id) {
            multiword_meaning_no = sqlite3_column_int(statement, 1);
            sqlite3_finalize(statement);
            
            std::string sql_text_str = "SELECT multiword_lemma_form, pos, eng_trans"+std::to_string(multiword_meaning_no)+" FROM multiword_lemmas WHERE multiword_id = "+std::to_string(multiword_id);
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_step(statement);

            const unsigned char* mw_lemma_form_rawsql = sqlite3_column_text(statement, 0);
            if(mw_lemma_form_rawsql != nullptr) mw_lemma_form = (const char*)mw_lemma_form_rawsql;
            
            const unsigned char* mw_lemma_meaning_rawsql = sqlite3_column_text(statement, 2);
            if(mw_lemma_meaning_rawsql != nullptr) mw_lemma_meaning = (const char*)mw_lemma_meaning_rawsql;
            
            pos = sqlite3_column_int(statement, 1);
            if(!pos) pos = 1;

            sqlite3_finalize(statement);
        }
        else sqlite3_finalize(statement);
    
        sqlite3_close(DB);

        std::ostringstream json;
        json << "{\"multiword_tag_content\":\"" << escapeQuotes(mw_lemma_form) << "\",\"multiword_textarea_content\":\"" << escapeQuotes(mw_lemma_meaning) << "\",\"multiword_meaning_no\":\"" << multiword_meaning_no << "\",\"multiword_id\":\"" << multiword_id << "\",\"pos\":\"" << pos << "\",\"adjacent_toknos\":" << adjacent_toknos_array.str() << "}";

        int content_length = json.str().size();
        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json.str();
        int length = post_response.str().size() + 1;

        sendToClient(clientSocket, post_response.str().c_str(), length);
        
       
        return true;
    }

    else {
        std::cout << "Database connection failed on retrieveMultiword()" << std::endl;
        return false;
    }

}

bool OcsServer::pullInMultiword(std::string _POST[2], int clientSocket) {

    sqlite3* DB;

    if(!sqlite3_open(m_DB_path, &DB)) {
        
        sqlite3_stmt* statement;

        int lang_id = safeStrToInt(_POST[1]);
        //there can be at most 10 words in a multiword-phrase so I can use int[10] instead of std::vector;
        int word_eng_ids[10] {0,0,0,0,0,0,0,0,0,0};

        std::string word_engine_id_str = "";

        short int word_eng_count = 0;
        for(auto i = _POST[0].begin(), nd=_POST[0].end(); i < nd && word_eng_count < 10; i++) {
            char c = (*i);
            if(c == ',') {
                int word_engine_id = std::stoi(word_engine_id_str);
                word_eng_ids[word_eng_count] = word_engine_id;
                word_eng_count++;
                word_engine_id_str = "";
                continue;
            }
            word_engine_id_str += c;
            if(nd - 1 == i) {
                int word_engine_id = std::stoi(word_engine_id_str);
                word_eng_ids[word_eng_count] = word_engine_id;
                word_eng_count++;
            }
        }

        short int null_count = 1;
        std::ostringstream sql_text_oss;
        sql_text_oss << "SELECT multiword_id FROM multiwords WHERE ";
        for(short int i = 0; i < word_eng_count; i++) {
            sql_text_oss << "word_eng_id" << (i + 1) << " = " << word_eng_ids[i];
            sql_text_oss << " AND ";
            null_count++;
        }
        for(; null_count < 11; null_count++) {
            sql_text_oss << "word_eng_id" << null_count << " = 0 AND ";
        }
        sql_text_oss << "lang_id = ?";
        
     
        //you cannot copy C-strings to variables like this:
        /* const char* sql_text = sql_text_oss.str().c_str(); */
        //you have to use strcpy() or in this case just don't assign it to a separate variable at all as it's unneccessary; when you use equals you are basically saying "construct a C-string out of the bytes of this memory-address-number"

        sqlite3_prepare_v2(DB, sql_text_oss.str().c_str(), -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, lang_id);
        sqlite3_step(statement);
        int multiword_id = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);

        std::cout << "multiword_id: " << multiword_id << std::endl; //debug

        std::string mw_lemma_form = "";
        std::string mw_lemma_meaning = "";
        short int pos = 1;
        if(multiword_id) {
            const char* sql_text = "SELECT multiword_lemma_form, pos, eng_trans1 FROM multiword_lemmas WHERE multiword_id = ?";
            sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, multiword_id);
            sqlite3_step(statement);
            const unsigned char* mw_lemma_form_rawsql = sqlite3_column_text(statement, 0);
            if(mw_lemma_form_rawsql != nullptr) mw_lemma_form = (const char*)mw_lemma_form_rawsql;
            const unsigned char* mw_lemma_meaning_rawsql = sqlite3_column_text(statement, 2);
            if(mw_lemma_meaning_rawsql != nullptr) mw_lemma_meaning = (const char*)mw_lemma_meaning_rawsql;
            pos = sqlite3_column_int(statement, 1);

            sqlite3_finalize(statement);
        }
        sqlite3_close(DB);

        std::ostringstream json;
        json << "{\"multiword_tag_content\":\"" << escapeQuotes(mw_lemma_form) << "\",\"multiword_textarea_content\":\"" << escapeQuotes(mw_lemma_meaning) << "\",\"multiword_id\":\"" << multiword_id << "\",\"pos\":\"" << pos << "\"}";
        
        int content_length = json.str().size();
        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json.str();
        int length = post_response.str().size() + 1;

        sendToClient(clientSocket, post_response.str().c_str(), length);
        return true;
    }
    else {
        std::cout << "Database connection failed on pullInMultiword()" << std::endl;
        return false;
    }
}

bool OcsServer::pullInMultiwordByForm(std::string _POST[4], int clientSocket) {
    sqlite3* DB;
    if(!sqlite3_open(m_DB_path, &DB)) {
        sqlite3_stmt* statement1;
        
        int multiword_length = safeStrToInt(_POST[0]);
        std::string candidate_mw_lemma_form = URIDecode(_POST[1]);
        int lang_id = safeStrToInt(_POST[2]);
        int requested_pos = safeStrToInt(_POST[3]);

        std::cout << "Target MW length: " << multiword_length << "\nMW lemma-form to check for: " << candidate_mw_lemma_form << "\nLang ID: " << lang_id << "\n";
        
        int target_multiword_id = 0;
        std::string target_mw_meaning = "";
        int target_pos = 0;
        bool target_mw_of_correct_length_exists = false;

        if(requested_pos == 0) {
            const char* sql_text = "SELECT multiword_id, eng_trans1, pos FROM multiword_lemmas WHERE lang_id = ? AND multiword_lemma_form = ? ORDER BY multiword_id";
            sqlite3_prepare_v2(DB, sql_text, -1, &statement1, NULL);
            sqlite3_bind_int(statement1, 1, lang_id);
            sqlite3_bind_text(statement1, 2, candidate_mw_lemma_form.c_str(), -1, SQLITE_STATIC);
        }
        else {
            const char* sql_text = "SELECT multiword_id, eng_trans1, pos FROM multiword_lemmas WHERE lang_id = ? AND multiword_lemma_form = ? AND pos = ?";
            sqlite3_prepare_v2(DB, sql_text, -1, &statement1, NULL);
            sqlite3_bind_int(statement1, 1, lang_id);
            sqlite3_bind_text(statement1, 2, candidate_mw_lemma_form.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(statement1, 3, requested_pos);
        }

        sqlite3_step(statement1);
        target_multiword_id = sqlite3_column_int(statement1, 0);
        if(target_multiword_id) {
            const unsigned char* target_mw_meaning_rawsql = sqlite3_column_text(statement1, 1);
            if(target_mw_meaning_rawsql != nullptr) target_mw_meaning = (const char*)target_mw_meaning_rawsql;
            target_pos = sqlite3_column_int(statement1, 2);

            sqlite3_stmt* statement2;
            std::string sql_text_str = "SELECT multiword_id FROM multiwords WHERE multiword_id = ? AND word_eng_id"+std::to_string(multiword_length)+" != 0";
            if(multiword_length < 10) sql_text_str +=" AND word_eng_id"+std::to_string(multiword_length + 1)+" = 0";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement2, NULL);
            sqlite3_bind_int(statement2, 1, target_multiword_id);
            sqlite3_step(statement2);
            target_mw_of_correct_length_exists = sqlite3_column_int(statement2, 0); //implicit cast of int to bool

            sqlite3_finalize(statement2);

        }
        std::cout << "Multiword ID of candidate-phrase: " << target_multiword_id << "\neng_trans1: " << target_mw_meaning << "\nMW pos: " << target_pos << "\nLength of candidate-phrase matches target-length? " << target_mw_of_correct_length_exists << "\n";
        sqlite3_finalize(statement1);

        std::ostringstream json;
        if(target_mw_of_correct_length_exists) {
            json << "{\"multiword_textarea_content\":\"" << escapeQuotes(target_mw_meaning) << "\",\"multiword_id\":" << target_multiword_id << ",\"pos\":" << target_pos << "}";
        }
        else {
            json << "{\"multiword_textarea_content\":\"\",\"multiword_id\":0,\"pos\":0}";
        }
        
        
        int content_length = json.str().size();
        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json.str();
        int length = post_response.str().size() + 1;

        sendToClient(clientSocket, post_response.str().c_str(), length);
        

        sqlite3_close(DB);
        return true;
    }
    else {
        std::cout << "Database connection failed on pullInMultiwordByForm()\n";
        return false;
    }
}


bool OcsServer::recordMultiwordOld(std::string _POST[8], int clientSocket) {
    sqlite3* DB;

    if(!sqlite3_open(m_DB_path, &DB)) {
        sqlite3_stmt* statement;

        int word_eng_ids[10] {0,0,0,0,0,0,0,0,0,0};
        std::string word_engine_id_str = "";
        short int word_count = 0;
        for(auto i = _POST[0].begin(), nd=_POST[0].end(); i < nd && word_count < 10; i++) {
            char c = (*i);
            if(c == ',') {
                int word_engine_id = std::stoi(word_engine_id_str);
                word_eng_ids[word_count] = word_engine_id;
                word_count++;
                word_engine_id_str = "";
                continue;
            }
            word_engine_id_str += c;
            if(nd - 1 == i) {
                int word_engine_id = std::stoi(word_engine_id_str);
                word_eng_ids[word_count] = word_engine_id;
                word_count++;
            }
        }
        word_count = 0;
        sqlite3_int64 toknos[10] {0,0,0,0,0,0,0,0,0,0};
        std::string tokno_str = "";
        for(auto i = _POST[1].begin(), nd=_POST[1].end(); i < nd && word_count < 10; i++) {
            char c = (*i);
            if(c == ',') {
                sqlite3_int64 tokno = std::stol(tokno_str);
                toknos[word_count] = tokno;
                word_count++;
                tokno_str = "";
                continue;
            }
            tokno_str += c;
            if(nd - 1 == i) {
                sqlite3_int64 tokno = std::stol(tokno_str);
                toknos[word_count] = tokno;
                word_count++;
            }
        }

        std::string multiword_lemma_form = URIDecode(_POST[2]);
        std::cout << "MW lemma form: "<< multiword_lemma_form << std::endl;
        std::string multiword_lemma_meaning = URIDecode(_POST[3]);
        std::cout << "MW lemma meaning: " << multiword_lemma_meaning << std::endl;
        short int multiword_meaning_no = safeStrToInt(_POST[4]);
        short int pos = std::stoi(_POST[5]);
        int lang_id = std::stoi(_POST[6]);
        sqlite3_int64 anchor_tokno = std::stol(_POST[7]);

        const char *sql_BEGIN = "BEGIN IMMEDIATE";
        const char *sql_COMMIT = "COMMIT";
              
        sqlite3_prepare_v2(DB, sql_BEGIN, -1, &statement, NULL);
        sqlite3_step(statement);
        sqlite3_finalize(statement);

        const char* sql_text = "INSERT OR IGNORE INTO multiword_lemmas (multiword_lemma_form, pos, lang_id) VALUES (?, ?, ?)";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_text(statement, 1, multiword_lemma_form.c_str(), -1, SQLITE_STATIC); //SQLITE_TRANSIENT
        sqlite3_bind_int(statement, 2, pos);
        sqlite3_bind_int(statement, 3, lang_id);
        sqlite3_step(statement);
        sqlite3_finalize(statement);

        sql_text = "SELECT multiword_id FROM multiword_lemmas WHERE multiword_lemma_form = ? AND pos = ? AND lang_id = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_text(statement, 1, multiword_lemma_form.c_str(), -1, SQLITE_STATIC); //SQLITE_TRANSIENT
        sqlite3_bind_int(statement, 2, pos);
        sqlite3_bind_int(statement, 3, lang_id);
        sqlite3_step(statement);
        int multiword_id = sqlite3_column_int(statement, 0); //returns 0 if row is NULL anyway, which it won't be because I just INSERTED it
        sqlite3_finalize(statement);

        sql_text = "SELECT multiword_count FROM display_text WHERE tokno = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int64(statement, 1, anchor_tokno);
        sqlite3_step(statement);
        int multiword_count = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);

        if(!multiword_count) {
            sql_text = "SELECT MAX(multiword_count) AS max_count FROM display_text";
            sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_step(statement);
            multiword_count = sqlite3_column_int(statement, 0) + 1;
            sqlite3_finalize(statement);
        }
        else { //If the anchor-word (clicked-on word) already has a multiword-annotation, get rid of it first incase any of its constituent words have changed.
            sql_text = "UPDATE display_text SET multiword_id = NULL, multiword_meaning_no = NULL, multiword_count = NULL WHERE multiword_count = ?";
            sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, multiword_count);
            sqlite3_step(statement);
            sqlite3_finalize(statement);
        }
        
        sql_text = "UPDATE display_text SET multiword_id = ?, multiword_count = ? WHERE tokno = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, multiword_id);            
        sqlite3_bind_int(statement, 2, multiword_count);
        for(short int i = 0; i < word_count; i++) {
            sqlite3_bind_int64(statement, 3, toknos[i]);
            sqlite3_step(statement);
            sqlite3_reset(statement); //this should retain the bindings; we only need to change tokno so no need to rebind the others
        }
        sqlite3_finalize(statement);
        
        std::ostringstream sql_text_oss;
        sql_text_oss << "INSERT OR IGNORE INTO multiwords (multiword_id, ";
        for(short int i = 0; i < word_count; i++) {
            sql_text_oss << "word_eng_id" << (i + 1) << ", ";
        }
        sql_text_oss << "lang_id) VALUES (";
        for(short int i = 0; i < word_count+2; i++) {
            sql_text_oss << "?";
            if(i < word_count + 1) {
                sql_text_oss << ", ";
            }
        }
        sql_text_oss << ")";
        std::cout << sql_text_oss.str() << std::endl;
        sqlite3_prepare_v2(DB, sql_text_oss.str().c_str(), -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, multiword_id);
        for(short int i = 0; i < word_count; i++) {
            sqlite3_bind_int(statement, i+2, word_eng_ids[i]);
        }
        sqlite3_bind_int(statement, word_count+2, lang_id);

        std::cout << "INSERT multiwords run code: " << sqlite3_step(statement) << std::endl;
        sqlite3_finalize(statement);

        
        std::string sql_text_str = "UPDATE multiword_lemmas SET eng_trans"+std::to_string(multiword_meaning_no)+" = ? WHERE multiword_id = ?";
        std::cout << sql_text_str << std::endl;
        sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
        std::cout << "MW lemma meaning: " << multiword_lemma_meaning << std::endl;
        std::cout << "bind text code: " << sqlite3_bind_text(statement, 1, multiword_lemma_meaning.c_str(), -1, SQLITE_STATIC) << std::endl; //SQLITE_TRANSIENT
     
        sqlite3_bind_int(statement, 2, multiword_id);
        std::cout << "update meaning run code: " << sqlite3_step(statement) << std::endl;
        sqlite3_finalize(statement);
    
        sql_text = "UPDATE display_text SET multiword_meaning_no = ? WHERE multiword_count = ?";
        sqlite3_prepare(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, multiword_meaning_no);
        sqlite3_bind_int(statement, 2, multiword_count);
        sqlite3_step(statement);
        sqlite3_finalize(statement);
        
        sqlite3_exec(DB, sql_COMMIT, nullptr, nullptr, nullptr);
        sqlite3_close(DB);

        std::string responseText = std::to_string(multiword_count) + "," + std::to_string(multiword_id);
        int content_length = responseText.size(); //should be done with maths not strings obviously

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " << content_length << "\r\n\r\n" << responseText;

        int length = post_response.str().size() + 1;

        sendToClient(clientSocket, post_response.str().c_str(), length);

     
        return true;
    }
    else {
        std::cout << "Database connection failed on recordMultiword()" << std::endl; 
        return false;
    }
}

bool OcsServer::recordMultiword(std::string _POST[8], int clientSocket) {
    sqlite3* DB;

    if(!sqlite3_open(m_DB_path, &DB)) {
        sqlite3_stmt* statement;

        int word_eng_ids[10] {0,0,0,0,0,0,0,0,0,0};
        std::string word_engine_id_str = "";
        short int word_count = 0;
        for(auto i = _POST[0].begin(), nd=_POST[0].end(); i < nd && word_count < 10; i++) {
            char c = (*i);
            if(c == ',') {
                int word_engine_id = std::stoi(word_engine_id_str);
                word_eng_ids[word_count] = word_engine_id;
                word_count++;
                word_engine_id_str = "";
                continue;
            }
            word_engine_id_str += c;
            if(nd - 1 == i) {
                int word_engine_id = std::stoi(word_engine_id_str);
                word_eng_ids[word_count] = word_engine_id;
                word_count++;
            }
        }
        word_count = 0;
        sqlite3_int64 toknos[10] {0,0,0,0,0,0,0,0,0,0};
        std::string tokno_str = "";
        for(auto i = _POST[1].begin(), nd=_POST[1].end(); i < nd && word_count < 10; i++) {
            char c = (*i);
            if(c == ',') {
                sqlite3_int64 tokno = std::stol(tokno_str);
                toknos[word_count] = tokno;
                word_count++;
                tokno_str = "";
                continue;
            }
            tokno_str += c;
            if(nd - 1 == i) {
                sqlite3_int64 tokno = std::stol(tokno_str);
                toknos[word_count] = tokno;
                word_count++;
            }
        }

        std::string multiword_lemma_form = URIDecode(_POST[2]);
        std::cout << "MW lemma form: "<< multiword_lemma_form << std::endl;
        std::string multiword_lemma_meaning = URIDecode(_POST[3]);
        std::cout << "MW lemma meaning: " << multiword_lemma_meaning << std::endl;
        short int multiword_meaning_no = safeStrToInt(_POST[4]);
        short int pos = std::stoi(_POST[5]);
        int lang_id = std::stoi(_POST[6]);
        sqlite3_int64 anchor_tokno = std::stol(_POST[7]);

        const char *sql_BEGIN = "BEGIN IMMEDIATE";
        const char *sql_COMMIT = "COMMIT";

        sqlite3_exec(DB, sql_BEGIN, nullptr, nullptr, nullptr);

        const char* sql_text = "SELECT multiword_id FROM multiword_lemmas WHERE multiword_lemma_form = ? AND pos = ? AND lang_id = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_text(statement, 1, multiword_lemma_form.c_str(), -1, SQLITE_STATIC); //SQLITE_TRANSIENT
        sqlite3_bind_int(statement, 2, pos);
        sqlite3_bind_int(statement, 3, lang_id);
        sqlite3_step(statement);
        sqlite_int64 multiword_id = sqlite3_column_int64(statement, 0);
        sqlite3_finalize(statement);

        sql_text = "SELECT multiword_count FROM display_text WHERE tokno = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int64(statement, 1, anchor_tokno);
        sqlite3_step(statement);
        int multiword_count = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);
        bool existing_multiword_count = true;
        if(!multiword_count) {
            sql_text = "SELECT MAX(multiword_count) FROM display_text WHERE multiword_count IS NOT NULL"; //WHERE-clause necessary to trigger use of the partial-index which excludes the NULL values
            sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_step(statement);
            multiword_count = sqlite3_column_int(statement, 0) + 1;
            sqlite3_finalize(statement);
            //multiword_count = 583;
            existing_multiword_count = false;
        }

        if(!multiword_id) {
            sql_text = "SELECT MAX(multiword_id) FROM multiword_lemmas";
            sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_step(statement);
            sqlite3_int64 new_multiword_id = sqlite3_column_int(statement, 0) + 1;
            sqlite3_finalize(statement);

            std::string sql_text_str = "INSERT INTO multiword_lemmas (multiword_id, multiword_lemma_form, eng_trans"+std::to_string(multiword_meaning_no)+", pos, lang_id) VALUES (?, ?, ?, ?, ?)";
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            sqlite3_bind_int64(statement, 1, new_multiword_id);
            sqlite3_bind_text(statement, 2, multiword_lemma_form.c_str(), -1, SQLITE_STATIC); //SQLITE_TRANSIENT
            sqlite3_bind_text(statement, 3, multiword_lemma_meaning.c_str(), -1, SQLITE_STATIC); 
            sqlite3_bind_int(statement, 4, pos);
            sqlite3_bind_int(statement, 5, lang_id);
            sqlite3_step(statement);
            sqlite3_finalize(statement);

            multiword_id = new_multiword_id;
        }
        else {

            std::string sql_text_str = "UPDATE multiword_lemmas SET eng_trans"+std::to_string(multiword_meaning_no)+" = ? WHERE multiword_id = ?";
            std::cout << sql_text_str << std::endl;
            sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
            std::cout << "MW lemma meaning: " << multiword_lemma_meaning << std::endl;
            sqlite3_bind_text(statement, 1, multiword_lemma_meaning.c_str(), -1, SQLITE_STATIC); //SQLITE_TRANSIENT
            sqlite3_bind_int64(statement, 2, multiword_id);
            std::cout << "update meaning run code: " << sqlite3_step(statement) << std::endl;
            sqlite3_finalize(statement);
        }
 
        //the bottle-neck in this function was slow lookups of the display_text table via the non-indexed multiword_count column; I've added a partial-index (excludes the NULL rows) to the display_text table on the multiword_count column to speed it up

        if(existing_multiword_count) { //If the anchor-word (clicked-on word) already has a multiword-annotation, get rid of it first incase any of its constituent words have changed.
            sql_text = "UPDATE display_text SET multiword_id = NULL, multiword_meaning_no = NULL, multiword_count = NULL WHERE multiword_count = ?";
            sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, multiword_count);
            sqlite3_step(statement);
            sqlite3_finalize(statement);
        } 
        
        sql_text = "UPDATE display_text SET multiword_id = ?, multiword_count = ? WHERE tokno = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int64(statement, 1, multiword_id);            
        sqlite3_bind_int(statement, 2, multiword_count);
        for(short int i = 0; i < word_count; i++) {
            sqlite3_bind_int64(statement, 3, toknos[i]);
            sqlite3_step(statement);
            sqlite3_reset(statement); //this should retain the bindings; we only need to change tokno so no need to rebind the others
        }
        sqlite3_finalize(statement);

        sql_text = "UPDATE display_text SET multiword_meaning_no = ? WHERE multiword_count = ?";
        sqlite3_prepare(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, multiword_meaning_no);
        sqlite3_bind_int(statement, 2, multiword_count);
        sqlite3_step(statement);
        sqlite3_finalize(statement);
        
        std::ostringstream sql_text_oss;
        sql_text_oss << "INSERT OR IGNORE INTO multiwords (multiword_id, ";
        for(short int i = 0; i < word_count; i++) {
            sql_text_oss << "word_eng_id" << (i + 1) << ", ";
        }
        sql_text_oss << "lang_id) VALUES (";
        for(short int i = 0; i < word_count+2; i++) {
            sql_text_oss << "?";
            if(i < word_count + 1) {
                sql_text_oss << ", ";
            }
        }
        sql_text_oss << ")";
        //std::cout << sql_text_oss.str() << std::endl;
        sqlite3_prepare_v2(DB, sql_text_oss.str().c_str(), -1, &statement, NULL);
        sqlite3_bind_int64(statement, 1, multiword_id);
        for(short int i = 0; i < word_count; i++) {
            sqlite3_bind_int(statement, i+2, word_eng_ids[i]);
        }
        sqlite3_bind_int(statement, word_count+2, lang_id);

        std::cout << "INSERT multiwords run code: " << sqlite3_step(statement) << std::endl;
        sqlite3_finalize(statement);        

        sqlite3_exec(DB, sql_COMMIT, nullptr, nullptr, nullptr);
        sqlite3_close(DB);

        std::string responseText = std::to_string(multiword_count) + "," + std::to_string(multiword_id);
        int content_length = responseText.size(); //should be done with maths not strings obviously

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " << content_length << "\r\n\r\n" << responseText;

        int length = post_response.str().size() + 1;

        sendToClient(clientSocket, post_response.str().c_str(), length);
     
        return true;
    }
    else {
        std::cout << "Database connection failed on recordMultiword()" << std::endl; 
        return false;
    }
}

bool OcsServer::updateMultiwordTranslations(std::string _POST[3], int clientSocket) {

    sqlite3* DB;

    if(!sqlite3_open(m_DB_path, &DB)) {
        sqlite3_stmt* statement;
        int multiword_id = std::stoi(_POST[0]);
        short int multiword_meaning_no = safeStrToInt(_POST[1]);
        std::string eng_trans = URIDecode(_POST[2]);

        std::string sql_text_str = "UPDATE multiword_lemmas SET eng_trans"+std::to_string(multiword_meaning_no)+" = ? WHERE multiword_id = ?";
        sqlite3_prepare_v2(DB, sql_text_str.c_str(), -1, &statement, NULL);
        if(eng_trans == "") sqlite3_bind_null(statement, 1);
        else sqlite3_bind_text(statement, 1, eng_trans.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(statement, 2, multiword_id);

        sqlite3_step(statement);
        sqlite3_finalize(statement);
        sqlite3_close(DB);

        std::string post_response_str = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
        int length =  post_response_str.size() + 1;
        sendToClient(clientSocket, post_response_str.c_str(), length);

        return true;
    }
    else {
        std::cout << "Database connection failed on updateMultiwordTranslations()\n" << std::endl;
        return false;
    }
}

bool OcsServer::deleteMultiword(std::string _POST[4], int clientSocket) {
    sqlite3* DB;

    if(!sqlite3_open(m_DB_path, &DB)) {
        sqlite3_stmt* statement1;
        sqlite3_stmt* statement2;

        int multiword_id = std::stoi(_POST[0]);

        int word_eng_ids[10] {0,0,0,0,0,0,0,0,0,0};
        sqlite3_int64 toknos[10] {0,0,0,0,0,0,0,0,0,0};
        
        int lang_id = std::stoi(_POST[3]);

        short int word_count = 0;
        std::string word_engine_id_str = "";
        for(auto i = _POST[1].begin(), nd = _POST[1].end(); i < nd && word_count < 10; i++) {
           char c = (*i);
            if(c == ',') {
                int word_engine_id = std::stoi(word_engine_id_str);
                word_eng_ids[word_count] = word_engine_id;
                word_count++;
                word_engine_id_str = "";
                continue;
            }
            word_engine_id_str += c;
            if(nd - 1 == i) {
                int word_engine_id = std::stoi(word_engine_id_str);
                word_eng_ids[word_count] = word_engine_id;
                word_count++;
            }
        }
        word_count = 0;

        std::string tokno_str = "";
        for(auto i = _POST[2].begin(), nd = _POST[2].end(); i < nd && word_count < 10; i++) {
            char c = (*i);
            if(c == ','){
                sqlite3_int64 tokno = std::stol(tokno_str);
                toknos[word_count] = tokno;
                word_count++;
                tokno_str = "";
                continue;
            }
            tokno_str += c;
            if(nd - 1 == i) {
                sqlite3_int64 tokno = std::stol(tokno_str);
                toknos[word_count] = tokno;
                word_count++;
            }
        }

        const char *sql_BEGIN = "BEGIN";
        const char *sql_COMMIT = "COMMIT";

        sqlite3_exec(DB, sql_BEGIN, nullptr, nullptr, nullptr);

        std::ostringstream sql_oss1;
        std::ostringstream sql_oss2;
        sql_oss1 << "UPDATE display_text SET multiword_id = NULL, multiword_meaning_no = NULL, multiword_count = NULL WHERE ";
        //NON-INDEXED LOOKUP
        sql_oss2 << "SELECT multiword_count FROM display_text WHERE multiword_id = ? AND (";

        for(int i = 0; i < word_count; i++) {
            sql_oss1 << "tokno = ? ";
            sql_oss2 << "word_engine_id = ?";

            if(i + 1 < word_count) {
                sql_oss1 << " OR ";
                sql_oss2 << " OR ";
            }
        }
        sql_oss2 << ")";

        sqlite3_prepare_v2(DB, sql_oss1.str().c_str(), -1, &statement1, NULL);
        sqlite3_prepare_v2(DB, sql_oss2.str().c_str(), -1, &statement2, NULL);

        sqlite3_bind_int(statement2, 1, multiword_id);
        
        for(int i = 0; i < word_count; i++) {
            sqlite3_bind_int64(statement1, i + 1, toknos[i]);
            sqlite3_bind_int(statement2, i + 2, word_eng_ids[i]);
        }

        sqlite3_step(statement1);
        sqlite3_finalize(statement1);

        //std::vector<int> multiword_counts;
        //multiword_counts.reserve(100);

        //this checks whether there are any remaining instances of the just-deleted multiword_lemma left on the display_text with the same forms (word_eng_id) as the one we just deleted
        bool mw_leftover = false;
        int prev_mw_count = 0;
        int mw_count = 0;
        int x = 1;
        while(sqlite3_step(statement2) == SQLITE_ROW) {
           mw_count = sqlite3_column_int(statement2, 0);
           if(prev_mw_count == mw_count) x++; //x is counting the number of constituent words in each multiword-phrase
           if(x == word_count) { //if we find a multiword-phrase of the same length and consisting of the same forms as the one we just deleted, then we mustn't delete this particular form of the multiword_id from the multiwords table
               mw_leftover = true;
               break;
           }
           prev_mw_count = mw_count;
        }
        sqlite3_finalize(statement2);

        if(mw_leftover == false) {
            std::ostringstream sql_oss3;
            const char* sql_text = "DELETE FROM multiwords WHERE multiword_id = ? AND word_eng_id1 = ? AND word_eng_id2 = ? AND word_eng_id3 = ? AND word_eng_id4 = ? AND word_eng_id5 = ? AND word_eng_id6 = ? AND word_eng_id7 = ? AND word_eng_id8 = ? AND word_eng_id9 = ? AND word_eng_id10 = ? AND lang_id = ?";

            sqlite3_prepare_v2(DB, sql_text, -1, &statement1, NULL);
            sqlite3_bind_int(statement1, 1, multiword_id);

            for(int i = 0; i < 10; i++) {
                sqlite3_bind_int(statement1, i + 2, word_eng_ids[i]);
            }
            sqlite3_bind_int(statement1, 12, lang_id);

            sqlite3_step(statement1);
            sqlite3_finalize(statement1);

            sql_text = "SELECT multiword_id FROM multiwords WHERE multiword_id = ?";
            sqlite3_prepare_v2(DB, sql_text, -1, &statement1, NULL);
            sqlite3_bind_int(statement1, 1, multiword_id);
            
            if(sqlite3_step(statement1) != SQLITE_ROW) {
                sql_text = "DELETE FROM multiword_lemmas WHERE multiword_id = ?";
                sqlite3_prepare_v2(DB, sql_text, -1, &statement2, NULL);
                sqlite3_bind_int(statement2, 1, multiword_id);
                sqlite3_step(statement2);
                sqlite3_finalize(statement2);
            }
            sqlite3_finalize(statement1);

        }
        sqlite3_exec(DB, sql_COMMIT, nullptr, nullptr, nullptr);

        const char* POST_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
        sendToClient(clientSocket, POST_response, 64); //size manually counted, could've messed up

        sqlite3_close(DB);

        return true;
    }
    else {
        std::cout << "Database connection failed on deleteMultiword()\n" << std::endl;
        return false;
    }
}

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

bool OcsServer::disregardWord(std::string _POST[2], int clientSocket) {
    sqlite3* DB;

    if(!sqlite3_open(m_DB_path, &DB)) {

        sqlite3_stmt* statement;

        sqlite3_int64 tokno = std::stol(_POST[0]);
        int word_engine_id = std::stoi(_POST[1]);

        const char* sql_BEGIN = "BEGIN IMMEDIATE";
        const char* sql_COMMIT = "COMMIT";

        sqlite3_exec(DB, sql_BEGIN, nullptr, nullptr, nullptr);

        const char* sql_text = "UPDATE display_text SET word_engine_id = NULL WHERE tokno = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int64(statement, 1, tokno);
        sqlite3_step(statement);
        sqlite3_finalize(statement);
        
        sql_text = "DELETE FROM word_engine WHERE word_engine_id = ? AND first_lemma_id IS NULL";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        std::cout << "bind code: " << sqlite3_bind_int(statement, 1, word_engine_id) << std::endl;
        std::cout << "run code: " << sqlite3_step(statement) << std::endl;
        sqlite3_finalize(statement);

        std::string post_response = "HTTP/1.1 204 No Content\r\n\r\n";
        int length = post_response.size() + 1;
        sendToClient(clientSocket, post_response.c_str(), length); //not commiting the transaction until after the response is sent makes it seem much faster on the frontend, but it is cheeky and probably dangerous

        sqlite3_exec(DB, sql_COMMIT, nullptr, nullptr, nullptr);

        sqlite3_close(DB);
        return true;
    }
    else {
        std::cout << "Database connection failed on disregardWord()\n";
        return false;
    }
}

bool OcsServer::dumpLemmaTable(std::string _POST[1], int clientSocket) {
    sqlite3* DB;

    if(!sqlite3_open(m_DB_path, &DB)) {
        sqlite3_stmt* statement;
        int lang_id = std::stoi(_POST[0]);

        const char* sql_text = "SELECT lemma, eng_trans1, eng_trans2, eng_trans3, eng_trans4, eng_trans5, eng_trans6, eng_trans7, eng_trans8, eng_trans9, eng_trans10, pos FROM lemmas WHERE lang_id = ?";

        std::ostringstream json;
        json << "[[";

        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, lang_id);
        bool meanings_empty = true;
        bool lemmas_empty = true;
        while(sqlite3_step(statement) == SQLITE_ROW) {
            lemmas_empty = false;
            int pos = sqlite3_column_int(statement, 11);
            json << "{\"pos\":" << pos << ",\"lemma_form\":";
            
            const char* lemma_form;
            const unsigned char* lemma_rawsql = sqlite3_column_text(statement, 0);
            if(lemma_rawsql != nullptr) {
                lemma_form = (const char*)lemma_rawsql;
            }
            else lemma_form = "";
            json << "\"" << escapeQuotes(lemma_form) << "\",\"meanings\":{";

            meanings_empty = true;
            for(int i = 1; i < 11; i++) {
                const char* meaning;
                const unsigned char* meaning_rawsql = sqlite3_column_text(statement, i);
                if(meaning_rawsql != nullptr) {
                    meaning = (const char*)meaning_rawsql;
                    json << "\"" << i << "\":\"" << escapeQuotes(meaning) << "\",";
                    meanings_empty = false;
                }
                if(i == 10) {
                    if(meanings_empty == false) json.seekp(-1, std::ios_base::cur);
                    json << "}";
                }
            }
            json << "},";
        }
        
        if(lemmas_empty == false) json.seekp(-1, std::ios_base::cur);

        json << "],[";
        sqlite3_finalize(statement);

        sql_text = "SELECT multiword_lemma_form, eng_trans1, eng_trans2, eng_trans3, eng_trans4, eng_trans5, pos FROM multiword_lemmas WHERE lang_id = ?";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, lang_id);
        lemmas_empty = true;
        while(sqlite3_step(statement) == SQLITE_ROW) {
            lemmas_empty = false;
            int pos = sqlite3_column_int(statement, 6);
            json << "{\"pos\":" << pos << ",\"lemma_form\":";
            
            const char* mw_lemma_form;
            const unsigned char* mw_lemma_rawsql = sqlite3_column_text(statement, 0);
            if(mw_lemma_rawsql != nullptr) {
                mw_lemma_form = (const char*)mw_lemma_rawsql;
            }
            else mw_lemma_form = "";
            json << "\"" << escapeQuotes(mw_lemma_form) << "\",\"meanings\":{";

            meanings_empty = true;
            for(int i = 1; i < 6; i++) {
                const char* meaning;
                const unsigned char* meaning_rawsql = sqlite3_column_text(statement, i);
                if(meaning_rawsql != nullptr) {
                    meaning = (const char*)meaning_rawsql;
                    json << "\"" << i << "\":\"" << escapeQuotes(meaning) << "\",";
                    meanings_empty = false;
                }
                if(i == 5) {
                    if(meanings_empty == false) json.seekp(-1, std::ios_base::cur);
                    json << "}";
                }
            }
            json << "},";
        }
        if(lemmas_empty == false) json.seekp(-1, std::ios_base::cur);
        json << "]]";

        sqlite3_finalize(statement);

        int content_length = json.str().size();
        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json.str();

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);

        sqlite3_close(DB);
        return true;
    }
    else {
        std::cout << "Database connection failed on dumpLemmaTable()\n";
        return false;
    }
}

//this code-duplication is necessary to avoid the needless inefficiency of checking the lang_id every single time a word is processed
bool OcsServer::addTextOldEnglish(std::string _POST[3], int clientSocket) {
    std::cout << "update_db.php" << std::endl;
   
    UErrorCode status = U_ZERO_ERROR;
    std::string text_body = URIDecode(_POST[0]);

    sqlite3 *DB;
    sqlite3_stmt *statement;

    if(!sqlite3_open(m_DB_path, &DB)) {
        icu::UnicodeString eth, thorn;
        eth = eth.fromUTF8("ð");
        thorn = thorn.fromUTF8("þ");

        icu::BreakIterator *bi = icu::BreakIterator::createWordInstance(icu::Locale::getUS(), status);
        icu::UnicodeString regexp = "\\p{L}"; // from the ICU regex documentation: \p{UNICODE PROPERTY NAME}	Match any character having the specified Unicode Property. (L = letter). This will catch exactly the same things as what the BreakIterator views as word-boundaries, meaning words with thigns like the OCS abbreviation tilde which would get classified as non-words if we used \P{UNICODE PROPERTY NAME} (match characters NOT having the specified property). If the BreakIterator fucks up then this will fuck up, but in that case we were fucked anyway. possibly \w (match Unicode 'words') is better but that also matches numerals which I'm ambivalent about given there can be infinity numbers
        icu::RegexMatcher *matcher = new icu::RegexMatcher(regexp, 0, status); //call delete on this
        
        std::istringstream iss(text_body);

        int prep_code, run_code;
        std::string sql_text_str;
        const char *sql_text;

        int lang_id = std::stoi(_POST[2]);

        sql_text = "SELECT MAX(tokno) FROM display_text";
        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        run_code = sqlite3_step(statement);
             
        sqlite3_int64 dt_start = sqlite3_column_int64(statement, 0) + 1;
        sqlite3_finalize(statement);

        const char *sql_BEGIN = "BEGIN IMMEDIATE";
        const char *sql_COMMIT = "COMMIT";
              
        prep_code = sqlite3_prepare_v2(DB, sql_BEGIN, -1, &statement, NULL);
        run_code = sqlite3_step(statement);
        sqlite3_finalize(statement);

        for(std::string token; std::getline(iss, token, ' '); ) {
            icu::UnicodeString token_unicode = token.c_str();

            token_unicode.findAndReplace("'", "¬"); //the reason why even normal apostrophes need to be replaced is because the BreakIterator doesn't see them as word-boundaries, so it fucks up; it has nothing to do with SQLite using apostrophes as the string-marker. We restore the apostrophes before inserting them into the table, so we can get rid of icu::UnicodeString::findAndReplace() calls on the retrieveText() etc. functions

            bi->setText(token_unicode);

            int32_t start_range, end_range;
            icu::UnicodeString tb_copy;
            std::string word_eng_word;

            std::string text_word;

            int32_t p = bi->first();
            bool is_a_word = true;
            
            std::string punctuation = "";
            bool punct_empty = true;
            int newline_code = 1;
            bool word_punct = false;

            auto insertWord = [&]() {
                sqlite3_prepare_v2(DB, "INSERT OR IGNORE INTO word_engine (word, lang_id) VALUES (?, ?)", -1, &statement, NULL);
                sqlite3_bind_text(statement, 1, word_eng_word.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(statement, 2, lang_id);
                sqlite3_step(statement);
                sqlite3_finalize(statement);

                sqlite3_prepare_v2(DB, "INSERT INTO display_text (word_engine_id, text_word) SELECT word_engine_id, ? FROM word_engine WHERE word = ? AND lang_id = ?", -1, &statement, NULL);
                sqlite3_bind_text(statement, 1, text_word.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_text(statement, 2, word_eng_word.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(statement, 3, lang_id);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
            };

            auto insertPunct = [&]() {
                sqlite3_prepare_v2(DB, "INSERT INTO display_text (text_word) VALUES (?)", -1, &statement, NULL);
                sqlite3_bind_text(statement, 1, punctuation.c_str(), -1, SQLITE_STATIC);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
                punctuation = "";
                punct_empty = true;
            };
            auto updateNewline = [&]() {
                sqlite3_prepare_v2(DB, "UPDATE display_text SET space = ? WHERE tokno = last_insert_rowid()", -1, &statement, NULL);
                sqlite3_bind_int(statement, 1, newline_code);
                sqlite3_step(statement);
                sqlite3_finalize(statement);
                newline_code = 1;
            };

            while(p != icu::BreakIterator::DONE) {
                tb_copy = token_unicode;
                start_range = p;
                p = bi->next();
                end_range = p;
                if(end_range == -1) break;
                tb_copy.retainBetween(start_range, end_range);
                
                matcher->reset(tb_copy); //tells the RegexMatcher object to use tb_copy as its haystack
                if(matcher->find()) {
                    is_a_word = true;
                }
                else {
                    is_a_word = false;
                }
                
                if(is_a_word) {
                    if(punct_empty == false) {
                        insertPunct();
                    }
                    if(newline_code > 1) {
                        updateNewline();
                    }

                    tb_copy.toUTF8String(text_word);
                   
                    tb_copy.toLower();

                    //force word_engine forms to have thorn word-initially and eth otherwise
                    if(tb_copy.startsWith(eth)) tb_copy.replace(0, 1, thorn); 
                    tb_copy.findAndReplace(1, tb_copy.length() - 1, thorn, eth);
                    
                    tb_copy.toUTF8String(word_eng_word);

                    insertWord();
                }

                else {
                    tb_copy.toUTF8String(text_word);
                    if(text_word == "¬") text_word = "\'";

                    if(text_word == "\n") {
                        if(punct_empty == false) {
                            insertPunct();
                        }
                        newline_code++;
                    }
                    else {
                        if(newline_code > 1) {
                            updateNewline();
                        }
                        punctuation.append(text_word);
                        punct_empty = false;
                    }                
                }
                word_eng_word = ""; //toUTF8String appends the string in tb_copy to word_eng_word rather than overwriting it
                text_word = "";
            }

            if(punct_empty == false) {
                insertPunct();
            }

            if(newline_code > 1) {
                updateNewline();
            }

            sql_text_str = "UPDATE display_text SET space = 1 WHERE tokno = last_insert_rowid() AND space IS NULL";
            //  std::cout << sql_text_str << std::endl;
            sql_text = sql_text_str.c_str();
            prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
            run_code = sqlite3_step(statement);
            sqlite3_finalize(statement);
        }

        delete bi;
        delete matcher;
        
        icu::UnicodeString unicode_text_title = URIDecode(_POST[1]).c_str();
        std::string text_title;
       /* unicode_text_title.findAndReplace("’", "\'"); */
        unicode_text_title.toUTF8String(text_title);
        const char* text_title_c_str = text_title.c_str();
       

        sql_text = "INSERT INTO texts (text_title, dt_start, dt_end, lang_id) VALUES (?, ?, last_insert_rowid(), ?)";
        std::cout << "dt_start: " << dt_start << ", text_title: " << text_title << ", last_insert_row_id: " << sqlite3_last_insert_rowid(DB) << std::endl;

        prep_code = sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_bind_int64(statement, 2, dt_start);
        sqlite3_bind_text(statement, 1, text_title_c_str, -1, SQLITE_STATIC);
        sqlite3_bind_int(statement, 3, lang_id);

        run_code = sqlite3_step(statement);
        std::cout << "Finalize code: " << sqlite3_finalize(statement) << std::endl;
        std::cout << prep_code << " " << run_code << std::endl;

        sql_text = "SELECT text_id FROM texts WHERE text_id = last_insert_rowid()";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, NULL);
        sqlite3_step(statement);
        int new_text_id = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);
      
        prep_code = sqlite3_prepare_v2(DB, sql_COMMIT, -1, &statement, NULL);
        run_code = sqlite3_step(statement);
        std::cout << prep_code << " " << run_code << std::endl;

        sqlite3_finalize(statement);

        std::cout << "sqlite_close: " << sqlite3_close(DB) << std::endl;

        std::string POST_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\nSet-Cookie: text_id="+std::to_string(new_text_id)+"; Max-Age=157680000\r\nSet-Cookie: lang_id="+std::to_string(lang_id)+"; Max-Age=157680000\r\nSet-Cookie: current_pageno=1; Max-Age=157680000\r\n\r\n";
        int size = POST_response.size() + 1;

        sendToClient(clientSocket, POST_response.c_str(), size);
        return true;
    }
    else {
        std::cout << "Database connection failed on addTextOldEnglish()\n";
        return false;
    }   
}

bool OcsServer::updateDisplayWord(std::string _POST[5], int clientSocket) {

    sqlite3* DB;
    sqlite3_stmt* statement;

    if(!sqlite3_open(m_DB_path, &DB)) {
        sqlite3_int64 edit_tokno = std::stol(_POST[0]);
        int prev_word_eng_id = std::stoi(_POST[1]);
        int lang_id = std::stoi(_POST[2]);
        std::string new_displayTextWord = URIDecode(_POST[3]);
        std::string new_wordEngWord = URIDecode(_POST[4]);

        const char* sql_BEGIN = "BEGIN IMMEDIATE";
        const char* sql_COMMIT = "COMMIT";

        sqlite3_exec(DB, sql_BEGIN, nullptr, nullptr, nullptr);

        const char* sql = "INSERT OR IGNORE INTO word_engine (word, lang_id) VALUES (?, ?)";
        sqlite3_prepare_v2(DB, sql, -1, &statement, NULL);
        sqlite3_bind_text(statement, 1, new_wordEngWord.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(statement, 2, lang_id);
        std::cout << "INSERT run-code: " << sqlite3_step(statement) << std::endl;
        sqlite3_finalize(statement);

        sql = "SELECT word_engine_id, first_lemma_id FROM word_engine WHERE lang_id = ? AND word = ?";
        sqlite3_prepare_v2(DB, sql, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, lang_id);
        sqlite3_bind_text(statement, 2, new_wordEngWord.c_str(), -1, SQLITE_STATIC);
        std::cout << "SELECT word_engine_id run-code: " << sqlite3_step(statement) << std::endl;
        int new_word_eng_id = sqlite3_column_int(statement, 0);
        int new_first_lemma_id = sqlite3_column_int(statement, 1);
        sqlite3_finalize(statement);

        sql = "UPDATE display_text SET text_word = ?, word_engine_id = ?, lemma_id = NULL, lemma_meaning_no = NULL WHERE tokno = ?";
        sqlite3_prepare_v2(DB, sql, -1, &statement, NULL);
        sqlite3_bind_text(statement, 1, new_displayTextWord.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(statement, 2, new_word_eng_id);
        sqlite3_bind_int64(statement, 3, edit_tokno);
        std::cout << "update display_text statement run-code: " << sqlite3_step(statement) << std::endl;
        sqlite3_finalize(statement);

        std::ostringstream json_response;
        json_response << "{\"new_word_eng_id\":" << new_word_eng_id << ",\"new_first_lemma_id\":" << new_first_lemma_id << ",\"lemma_tt_data\":[";
        if(new_first_lemma_id) {
            sql = "SELECT lemma, eng_trans1, eng_trans2, eng_trans3, eng_trans4, eng_trans5, eng_trans6, eng_trans7, eng_trans8, eng_trans9, eng_trans10, pos FROM lemmas WHERE lemma_id = ?";
            std::string lemma_form = "";
            std::string lemma_trans = "";
            short int lemma_pos = 1;

            sqlite3_prepare_v2(DB, sql, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, new_first_lemma_id);
            sqlite3_step(statement);
            const unsigned char* lemma_rawsql = sqlite3_column_text(statement, 0);
            if (lemma_rawsql != nullptr) {
                lemma_form = (const char*)lemma_rawsql;
            }
            else lemma_form = "";

            lemma_trans = "";
            //this loop is necessary incase some bastard has decided to leave meaning_no1 blank but recorded meaning_no5 or whatever
            for (int i = 1; i < 11; i++) {
                const unsigned char* lemma_trans_rawsql = sqlite3_column_text(statement, i);
                if (lemma_trans_rawsql != nullptr) {
                    lemma_trans = (const char*)lemma_trans_rawsql;
                    break;
                }
            }
            lemma_pos = sqlite3_column_int(statement, 11);
            sqlite3_finalize(statement);

            json_response << "\"" << htmlspecialchars(lemma_form) << "\",\"" << htmlspecialchars(lemma_trans) << "\"," << lemma_pos;
        }
        json_response << "]}";

        std::cout << "updateDisplayWord() json_response: " << json_response.str() << std::endl;

        int content_length = json_response.str().size();
        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_response.str();

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
        //NON-INDEXED LOOKUP
        sql = "SELECT count(rowid) FROM display_text WHERE word_engine_id = ?";
        sqlite3_prepare_v2(DB, sql, -1, &statement, NULL);
        sqlite3_bind_int(statement, 1, prev_word_eng_id);
        sqlite3_step(statement);

        if(!sqlite3_column_int(statement, 0)) {
            sqlite3_finalize(statement);
            sql = "DELETE FROM word_engine WHERE word_engine_id = ?";
            sqlite3_prepare_v2(DB, sql, -1, &statement, NULL);
            sqlite3_bind_int(statement, 1, prev_word_eng_id);
            std::cout << "Previous word engine has been deleted with run-code: " << sqlite3_step(statement) << std::endl;
        }

        std::cout << "count(rowid) WHERE word_engine_id = " << prev_word_eng_id << ": " << sqlite3_column_int(statement, 0) << std::endl;
        sqlite3_finalize(statement);

        sqlite3_exec(DB, sql_COMMIT, nullptr, nullptr, nullptr);

        sqlite3_close(DB);
        return true;
    }
    else {
        std::cout << "Database connection failed on updateDisplayWord()\n";
        return false;
    }   
}

bool OcsServer::lemmaTooltipsMW(std::string _POST[3], int clientSocket) {
    sqlite3* DB;    

    if(!sqlite3_open(m_DB_path, &DB)) {

        sqlite3_stmt* statement1;
        sqlite3_stmt* statement2;
        sqlite3_stmt* statement3; 
        sqlite3_stmt* statement4;  
        sqlite3_stmt* statement5;
        sqlite3_stmt* statement6;    

        int tooltip_count = 1;
        //word_engine_id's are generally going to be smaller than toknos so more efficient to iterate through them
        for(auto i = _POST[1].begin(), nd=_POST[1].end(); i < nd; i++) {
            char c = (*i);
            if(c == ',') tooltip_count++;
        }
        int mw_tooltip_count = 1;
        for(auto i = _POST[2].begin(), nd=_POST[2].end(); i < nd; i++) {
            char c = (*i);
            if(c == ',') mw_tooltip_count++;
        }

        std::vector<sqlite3_int64> toknos;
        std::vector<int> word_engine_ids;

        toknos.reserve(tooltip_count); 
        word_engine_ids.reserve(tooltip_count);

        std::string tokno_str = "";
        for(auto i = _POST[0].begin(), nd=_POST[0].end(); i < nd; i++) {
            char c = (*i);
            if(c == ',') {
                sqlite3_int64 tokno = std::stol(tokno_str);
                toknos.emplace_back(tokno);
                tokno_str = "";
                continue;
            }
            tokno_str += c;
            if(nd - 1 == i) {
                sqlite3_int64 tokno = std::stol(tokno_str);
                toknos.emplace_back(tokno);
            }
        }
        std::string word_engine_id_str = "";
        for(auto i = _POST[1].begin(), nd=_POST[1].end(); i < nd; i++) {
            char c = (*i);
            if(c == ',') {
                int word_engine_id = std::stoi(word_engine_id_str);
                word_engine_ids.emplace_back(word_engine_id);
                word_engine_id_str = "";
                continue;
            }
            word_engine_id_str += c;
            if(nd - 1 == i) {
                int word_engine_id = std::stoi(word_engine_id_str);
                word_engine_ids.emplace_back(word_engine_id);
            }
        }
        
        std::vector<sqlite3_int64> mw_first_toknos;
        mw_first_toknos.reserve(mw_tooltip_count);
        std::string mw_first_tokno_str = "";
        for(auto i = _POST[2].begin(), nd=_POST[2].end(); i < nd; i++) {
            char c = (*i);
            if(c == ',') {
                sqlite3_int64 mw_first_tokno = std::stol(mw_first_tokno_str);
                mw_first_toknos.emplace_back(mw_first_tokno);
                mw_first_tokno_str = "";
                continue;
            }
            mw_first_tokno_str += c;
            if(nd - 1 == i) {
                sqlite3_int64 mw_first_tokno = std::stol(mw_first_tokno_str);
                mw_first_toknos.emplace_back(mw_first_tokno);
            }        
        }

        const char* sql_text1 = "SELECT lemma_id, lemma_meaning_no FROM display_text WHERE tokno = ?";
        const char* sql_text2 = "SELECT first_lemma_id FROM word_engine WHERE word_engine_id = ?";
        const char* sql_text3 = "SELECT lemma, eng_trans1, eng_trans2, eng_trans3, eng_trans4, eng_trans5, eng_trans6, eng_trans7, eng_trans8, eng_trans9, eng_trans10, pos FROM lemmas WHERE lemma_id = ?";
        sqlite3_prepare_v2(DB, sql_text1, -1, &statement1, NULL);
        sqlite3_prepare_v2(DB, sql_text2, -1, &statement2, NULL);
        sqlite3_prepare_v2(DB, sql_text3, -1, &statement3, NULL);

        std::string lemma_form = "";
        std::string lemma_trans = "";
        std::string mw_lemma_form = "";
        std::string mw_lemma_trans = "";
        short int pos = 1;
        short int mw_pos = 1;


        std::ostringstream json;
        json << "[[";

        int x = 0;
        for(const sqlite3_int64 &tokno : toknos) {

            sqlite3_bind_int64(statement1, 1, tokno);
            sqlite3_step(statement1);
            int lemma_id = sqlite3_column_int(statement1, 0);
 
            if(lemma_id) {
                short int lemma_meaning_no = sqlite3_column_int(statement1, 1);
                std::string sql_text4_str = "SELECT lemma, eng_trans"+std::to_string(lemma_meaning_no)+", pos FROM lemmas WHERE lemma_id = "+std::to_string(lemma_id); 
                sqlite3_prepare_v2(DB, sql_text4_str.c_str(), -1, &statement4, NULL);
                sqlite3_step(statement4);

                const unsigned char* lemma_rawsql = sqlite3_column_text(statement4, 0);
                if(lemma_rawsql != nullptr) {
                    lemma_form = (const char*)lemma_rawsql;
                }
                else lemma_form = "";
                const unsigned char* lemma_trans_rawsql = sqlite3_column_text(statement4, 1);
                if(lemma_trans_rawsql != nullptr) {
                    lemma_trans = (const char*)lemma_trans_rawsql;
                }
                else lemma_trans = "";
                pos = sqlite3_column_int(statement4, 2);

                std::cout << "lemma_form: " << lemma_form << ", lemma_trans: " << lemma_trans << ", pos: " << pos << std::endl;
                //sqlite3_reset(statement1);
                sqlite3_finalize(statement4);
            }
            else {
                sqlite3_bind_int(statement2, 1, word_engine_ids[x]);
                sqlite3_step(statement2);
                lemma_id = sqlite3_column_int(statement2, 0);
                std::cout << "lemma_id: " << lemma_id << std::endl;
                sqlite3_reset(statement2);

                sqlite3_bind_int(statement3, 1, lemma_id);
                sqlite3_step(statement3);
                const unsigned char* lemma_rawsql = sqlite3_column_text(statement3, 0);
                if(lemma_rawsql != nullptr) {
                    lemma_form = (const char*)lemma_rawsql;
                }
                else lemma_form = "";

                lemma_trans = "";
                for(int i = 1; i < 11; i++) {
                    const unsigned char* lemma_trans_rawsql = sqlite3_column_text(statement3, i);
                    if(lemma_trans_rawsql != nullptr) {
                        lemma_trans = (const char*)lemma_trans_rawsql;
                        break;
                    }
                }
                pos = sqlite3_column_int(statement3, 11);
                sqlite3_reset(statement3);
                std::cout << "lemma_form: " << lemma_form << ", lemma_trans: " << lemma_trans << ", pos: " << pos << std::endl;     
            }
            //I'm sure these need to be htmlspecialchars()'d as well/instead (as that escapes quotes with html codes)
            json << "{\"lemma_form\":\"" << htmlspecialchars(lemma_form) << "\",\"lemma_trans\":\"" << htmlspecialchars(lemma_trans) << "\",\"pos\":\"" << pos << "\"}";
            x++;
            if(x != tooltip_count) {
                json << ",";
            } //for some reason the browser only recognises the response as JSON if there is no trailing comma
            sqlite3_reset(statement1);
        } 
        sqlite3_finalize(statement1);
        sqlite3_finalize(statement2);
        sqlite3_finalize(statement3);

        json << "],[";
        
        const char* sql_text5 = "SELECT multiword_id, multiword_meaning_no FROM display_text WHERE tokno = ?";
        sqlite3_prepare_v2(DB, sql_text5, -1, &statement5, NULL);

        x = 0;
        for(const sqlite3_int64 &mw_tokno : mw_first_toknos) {
            sqlite3_bind_int64(statement5, 1, mw_tokno);
            sqlite3_step(statement5);
            int mw_first_id = sqlite3_column_int(statement5, 0);
            int mw_meaning_no = sqlite3_column_int(statement5, 1);
            if(!mw_meaning_no) mw_meaning_no = 1;

            if(mw_first_id) {

                std::string sql_text6_str = "SELECT multiword_lemma_form, pos, eng_trans"+std::to_string(mw_meaning_no)+" FROM multiword_lemmas WHERE multiword_id = ?"; //this string interpolation is allowed because mw_meaning_no is guaranteed to be an integer.
                sqlite3_prepare_v2(DB, sql_text6_str.c_str(), -1, &statement6, NULL);
                sqlite3_bind_int(statement6, 1, mw_first_id);
                sqlite3_step(statement6);

                const unsigned char* mw_lemma_rawsql = sqlite3_column_text(statement6, 0);
                if(mw_lemma_rawsql != nullptr) {
                    mw_lemma_form = (const char*)mw_lemma_rawsql;
                }
                else mw_lemma_form = "";
                const unsigned char* mw_trans_rawsql = sqlite3_column_text(statement6, 2);
                if(mw_trans_rawsql != nullptr) {
                    mw_lemma_trans = (const char*)mw_trans_rawsql;
                }
                else mw_lemma_trans = "";
                mw_pos = sqlite3_column_int(statement6, 1);

                sqlite3_finalize(statement6);

                std::cout << "mw_form: " << mw_lemma_form << "| mw_trans: " << mw_lemma_trans << "| mw_pos: " << mw_pos << "\n";
            }
            else {
                std::cout << "malformed or incorrect tokno\n";
                mw_pos = 1;
            }

            json << "{\"mw_lemma_form\":\"" << htmlspecialchars(mw_lemma_form) << "\",\"mw_lemma_trans\":\"" << htmlspecialchars(mw_lemma_trans) << "\",\"mw_pos\":\"" << mw_pos << "\"}";
            x++;
            if(x != mw_tooltip_count) {
                json << ",";
            } //for some reason the browser only recognises the response as JSON if there is no trailing comma
            sqlite3_reset(statement5);
        }
        sqlite3_finalize(statement5);
        
        json << "]]";

        sqlite3_close(DB);
        int content_length = json.str().size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json.str();
        std::cout << json.str() << "\n";
        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
       
        return true;
    }
    else {
        std::cout << "Database connection failed in lemmaTooltips()" << std::endl;
        return false;
    }
}

bool OcsServer::retrieveTextFromSearch(std::string _POST[1], int clientSocket) {

    int result_tokno = safeStrToInt(_POST[0]);

    int sentences_per_page = 30;
    std::ostringstream page_toknos_arr;
    page_toknos_arr << "[";
    int pageno_count = 1;
    int sentence_count = 0;

    int result_pageno = 1;
    bool pageno_found = false;

    sqlite3* DB;
    sqlite3_stmt* statement;

    if (!sqlite3_open("chu.db", &DB)) {

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
            sentence_count++;

            if (sentence_count % sentences_per_page == 0) {
                page_toknos_arr << "," << sentence_start_tokno - 1 << "],[" << sentence_start_tokno;
                page_toknos_vec.push_back(sentence_start_tokno);
                if (pageno_found == false && result_tokno <= sentence_start_tokno) {
                    result_pageno = pageno_count;
                    pageno_found = true;
                }
                pageno_count++;
            }
        }
        if(pageno_found == false) result_pageno = pageno_count; //the final page will contain less than 30 sentences so that modulo condition will never be fulfilled

        page_toknos_vec.push_back(tokno_end + 1);
        page_toknos_arr << "," << tokno_end << "]]";
        m_page_toknos_arr = page_toknos_arr.str();
        m_pageno_count = pageno_count;
        sqlite3_finalize(statement);

        m_cookies[1] = std::to_string(result_pageno);

        std::ostringstream html;
        html << "<br><div id=\"textbody\">";
        writeTextBody(html, DB, page_toknos_vec[result_pageno - 1], page_toknos_vec[result_pageno] - 1);

        html << "</div>";

        if (m_pageno_count > 1) {
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
        std::cout << m_page_toknos_arr << "\n";

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
        std::string json_str = "[]";
        int content_length = json_str.size();

        std::ostringstream post_response;
        post_response << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " << content_length << "\r\n\r\n" << json_str;

        int length = post_response.str().size() + 1;
        sendToClient(clientSocket, post_response.str().c_str(), length);
        return true;
    }
  
    
    if(!sqlite3_open("chu.db", &DB)) {
        

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
                text_content_result_oss << "\"";

                sqlite3_bind_int(statement2, 1, tokno + 5);
                sqlite3_bind_int(statement2, 2, tokno - 5);
                sqlite3_bind_int(statement2, 3, sentence_no);
                while(sqlite3_step(statement2) == SQLITE_ROW) {
                    text_content_result_oss << (const char*)sqlite3_column_text(statement2, 2);
                    if(sqlite3_column_int(statement2, 0) == tokno) {
                        text_content_result_oss << "<span class=\\\"result_word\\\">" + htmlspecialchars((const char*)sqlite3_column_text(statement2, 1)) + "</span>";
                    }
                    else text_content_result_oss << htmlspecialchars((const char*)sqlite3_column_text(statement2, 1));
                    
                    text_content_result_oss << (const char*)sqlite3_column_text(statement2, 3);
                }
                text_content_result_oss << "\"";
                text_results << text_content_result_oss.str() << ",";

                lcs_result_str.clear();
                sqlite3_reset(statement2);
                sqlite3_clear_bindings(statement2);
                results_count++;
            }
        }
                
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
        std::cout << "Database connection failed in lcsRegexSearch()" << std::endl;
        return false;
    }
}

bool OcsServer::greekTooltips(std::string _POST[2], int clientSocket) {
    sqlite3* DB;    
    
    if(!sqlite3_open("chu.db", &DB)) {

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