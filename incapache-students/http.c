/*
 * incApache_http.c: implementazione del protocollo HTTP per il web server
 *                   del corso di SET
 *
 * Programma sviluppato a supporto del laboratorio di
 * Sistemi di Elaborazione e Trasmissione del corso di laurea
 * in Informatica classe L-31 presso l'Universita` degli Studi di
 * Genova, anno accademico 2017/2018.
 *
 * Copyright (C) 2012-2014, 2016 by Giovanni Chiola <chiolag@acm.org>
 * Copyright (C) 2015-2016 by Giovanni Lagorio <giovanni.lagorio@unige.it>
 * Copyright (C) 2017 by Giovanni Chiola <chiolag@acm.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "incApache.h"

#define OptimizeTCP

#define RESPONSE_CODE_OK		200
#define RESPONSE_CODE_NOT_MODIFIED	304
#define RESPONSE_CODE_BAD_REQUEST	400
#define RESPONSE_CODE_NOT_FOUND		404

ssize_t send_all(int fd, const char *ptr, size_t n, int flags)
{
	size_t n_left = n;
	while (n_left > 0) {
		ssize_t n_written = send(fd, ptr, n_left, flags);
		if (n_written < 0) {
			if (n_left == n)
				return -1; /* nothing has been sent */
			else
				break; /* we have sent something */
		} else if (n_written == 0) {
			break;
		}
		n_left -= n_written;
		ptr += n_written;
	}
	assert(n - n_left >= 0);
	return n - n_left;
}

void send_response(int client_fd, int response_code,
#ifdef INCaPACHE_2_3
		   int is_http1_0, int thread_no,
#endif
		   char *filename, struct stat *stat_p)
{
	time_t now_t = time(NULL);
	struct tm now_tm;
	time_t file_modification_time;
	struct tm file_modification_tm;
	char time_as_string[MAX_TIME_STR];
	char http_header[MAX_HEADER_SIZE] = "HTTP/1.";
	size_t header_size;
	int fd = -1;
	off_t file_size = 0;
	char *mime_type = NULL;
	const char * const HTML_mime = "text/html";
	struct stat stat_buffer;
	size_t header_sent;
	debug("  ... start send_response(response_code=%d, filename=%s)\n", response_code, filename);

	/*** Compute date of servicing current HTTP Request using a variant of gmtime() ***/
/*** TO BE DONE 2.2 START ***/
    if(gmtime_r(&now_t, &now_tm)==NULL)fail_errno("Couldn't convert data");
/*** TO BE DONE 2.2 END ***/

	strftime(time_as_string, MAX_TIME_STR, "%a, %d %b %Y %T GMT", &now_tm);
#ifdef INCaPACHE_2_3
	strcat(http_header, is_http1_0 ? "0 " : "1 ");
#else /* #ifdef INCaPACHE_2_3 */
	strcat(http_header, "0 ");
#endif /* #ifdef INCaPACHE_2_3 */
	switch (response_code) {
	case RESPONSE_CODE_OK:
		strcat(http_header, "200 OK");
		if (filename != NULL) {
			if (stat_p != NULL) {
				fd = open(filename, O_RDONLY);
				if (fd<0)
					fail("send_response: cannot open file for reading (has the file vanished?)");
				debug("    ... send_response(%d, %s): opened file %d\n", response_code, filename, fd);
			}
			mime_type = get_mime_type(filename);
			debug("    ... send_response(%d, %s): got mime type %s\n", response_code, filename, mime_type);
			if (stat_p == NULL) {
				stat_p = &stat_buffer;
				if (stat(filename, stat_p))
					fail_errno("stat");
			}

			/*** compute file_size and file_modification_time ***/
/*** TO BE DONE 2.2 START ***/
        file_size=stat_p->st_size;
        file_modification_time=stat_p->st_mtime;
        if(gmtime_r(&file_modification_time, &file_modification_tm)==NULL)fail_errno("gmtime");
/*** TO BE DONE 2.2 END ***/

			debug("      ... send_response(%3d,%s) : file opened, size=%lu, mime=%s\n", response_code, filename, (unsigned long)file_size, mime_type);
		}
		break;
	case RESPONSE_CODE_NOT_MODIFIED:
		strcat(http_header, "304 Not Modified");
		break;
	case RESPONSE_CODE_BAD_REQUEST:
		strcat(http_header, "400 Bad Request");
		break;
	case RESPONSE_CODE_NOT_FOUND:
		strcat(http_header, "404 Not Found");
		if ((fd = open(HTML_404, O_RDONLY)) >= 0) {

			/*** compute file_size, mime_type, and file_modification_time of HTML_404 ***/
/*** TO BE DONE 2.2 START ***/
            if (stat_p == NULL) /*copiato da sopra ... cambiare mess errore*/
            {
				stat_p = &stat_buffer;
				if (stat(HTML_404, stat_p)){fail_errno("stat");}
			}
            mime_type = get_mime_type(HTML_404);
            file_size=stat_p->st_size;
            file_modification_time=stat_p->st_mtime;
            if(gmtime_r(&file_modification_time, &file_modification_tm)==NULL)fail_errno("gmtime");
/*** TO BE DONE 2.2 END ***/

		}
		break;
	default:
		strcat(http_header, "501 Method Not Implemented\r\nAllow: HEAD,GET");
		if ((fd = open(HTML_501, O_RDONLY)) >= 0) {

			/*** compute file_size, mime_type, and file_modification_time of HTML_501 ***/
/*** TO BE DONE 2.2 START ***/
            if (stat_p == NULL) /*copiato da sopra ... cambiare mess errore*/
            {
				stat_p = &stat_buffer;
				if (stat(HTML_501, stat_p)){fail_errno("stat");}
			}
            mime_type = get_mime_type(HTML_501);
            file_size=stat_p->st_size;
            file_modification_time=stat_p->st_mtime;
            if(gmtime_r(&file_modification_time, &file_modification_tm)==NULL)fail_errno("gmtime");
/*** TO BE DONE 2.2 END ***/

		}
		break;
	}
	strcat(http_header, "\r\nDate: ");
	strcat(http_header, time_as_string);
	strcat(http_header, "\r\nServer: incApache for SET.\r\n");
#ifdef INCaPACHE_2_3
	if (response_code == 501 || is_http1_0)
#endif
		strcat(http_header, "Connection: close\r\n");
	if (file_size > 0 && mime_type != NULL) {
		sprintf(http_header + strlen(http_header), "Content-Length: %lu \r\nContent-Type: %s\r\nLast-Modified: ", (unsigned long)file_size, mime_type);

		/*** compute time_as_string, corresponding to file_modification_time, in GMT standard format;
		     see gmtime and strftime ***/
/*** TO BE DONE 2.2 START ***/

/*alla linea 78 non fa già questa operazione? Quello alla linea 78 è la data di richiesta*/
        strftime(time_as_string, MAX_TIME_STR, "%a, %d %b %Y %T GMT", &file_modification_tm);
/*** TO BE DONE 2.2 END ***/

		strcat(http_header, time_as_string);
		strcat(http_header, "\r\n");
	}
	strcat(http_header, "\r\n");
	debug("      ... send_response(%d, %s) : header prepared\n", response_code, filename);
	printf("Sending the following response:\n%s\n",http_header);
	header_size = strlen(http_header);
#ifdef INCaPACHE_2_3
	join_prev_thread(thread_no);
#endif
#ifdef OptimizeTCP
	if ((header_sent=send_all(client_fd, http_header, header_size, (fd >= 0 ? MSG_MORE : 0))) < header_size)
#else
	if ((header_sent=send_all(client_fd, http_header, header_size, 0)) < header_size )
#endif
	{
		if (header_sent==-1)
			fail_errno("incApache: could not send HTTP header");
		debug("header partially sent; header_size=%lu, header_sent=%lu\n", (unsigned long) header_size, (unsigned long) header_sent);
		close(fd);
		fd = -1;
	}
	debug("      ... send_response(%d, %s): header sent\n", response_code, filename);
	if (fd >= 0) {

		/*** send fd file on client_fd, then close fd; see syscall sendfile  ***/
/*** TO BE DONE 2.2 START ***/

    if(sendfile(client_fd,fd,NULL,stat_p->st_size)<0)/*dal manuale:Note that a  successful
       call to sendfile() may write fewer bytes than requested; the caller should be prepared to retry the call if
       there were unsent bytes.   Devo fare un controllo aggiuntivo quindi?*/
    {/*errore*/}

/*** TO BE DONE 2.2 END ***/

	}
#ifdef OptimizeTCP
	if (fd >= 0) {
		int optval = 1;
		if (setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(int)))
			fail_errno("Cannot set socket options");
	}
#endif
	debug("  ... end send_response(%d, %s)\n", response_code, filename);
	free(mime_type);
}

void manage_http_requests(int client_fd
#ifdef INCaPACHE_2_3
		, int connection_no
#endif
)
{
#define METHOD_NONE		0
#define METHOD_HEAD		1
#define METHOD_GET		2
#define METHOD_NOT_CHANGED	3
#define METHOD_CONDITIONAL	4
#define MethodIsConditional(m) ((m)&METHOD_CONDITIONAL)
	FILE *client_stream = fdopen(client_fd, "r");
	char *http_request_line = NULL;
	char *strtokr_save;
	size_t n = 0;
	int http_method;
	struct tm since_tm;
	struct stat *stat_p;
#ifdef INCaPACHE_2_3
	int is_http1_0 = 0;
	int thread_idx;
#endif
	if (!client_stream)
		fail_errno("cannot open client stream");
	while (getline(&http_request_line, &n, client_stream) >= 0) {
		char *method_str, *filename, *protocol;
		char *http_option_line = NULL;
		char *option_name, *option_val;
		printf("\n-----------------------------------------------\n");
		printf("Received the following request:\n");
		printf("%s", http_request_line);
#ifdef INCaPACHE_2_3
		thread_idx = find_unused_thread_idx(connection_no);
#endif

		/*** parse first line defining the 3 strings method_str,
		 *** filename, and protocol ***/
/*** TO BE DONE 2.2 START ***/
        char* pos=http_request_line;
        char* blankpos = strchr(http_request_line, ' ');
        method_str = strndup(pos, blankpos-pos);
        pos=blankpos+1;	
        blankpos = strchr(pos, ' ');
        filename=strndup(pos,blankpos-pos);
        pos=blankpos+1;
        blankpos = strchr(pos, '\n');
        protocol=strndup(pos, 8);
        if(protocol[strlen(protocol)-1]=='\n') protocol[strlen(protocol)-1]='\0';
        /*gestione garbage*/
    /*se c'è del garbage dopo cosa faccio?ignoro o segnalo errore?*/
/*** TO BE DONE 2.2 END ***/

		debug("   ... method_str=%s, filename=%s (0=%c), protocol=%s (len=%d)\n",
		      method_str, filename, filename ? filename[0] : '?', protocol, (int)(protocol ? strlen(protocol) : 0));
		if (method_str == NULL || filename == NULL || protocol == NULL ||/*controlla i parametri acquisiti*/
		    filename[0] != '/' || strncmp(protocol, "HTTP/1.", 7) != 0 ||
		    strlen(protocol) != 8) {
			debug("       ... Bad Request!\n");
			SEND_RESPONSE(client_fd, RESPONSE_CODE_BAD_REQUEST,/*risp con bad req*/
#ifdef INCaPACHE_2_3
				      1, connection_no, thread_idx,
#endif
				      NULL, NULL);
			free(http_request_line);
			break;
		}
#ifdef INCaPACHE_2_3
		is_http1_0 = !strcmp(protocol, "HTTP/1.0");
#endif
		memset(&since_tm, 0, sizeof(since_tm));
		http_method = METHOD_NONE;/*setta il metodo default e succ ne cerca uno specifico*/
		if (strcmp(method_str, "GET") == 0)
			http_method = METHOD_GET;
		else if (strcmp(method_str, "HEAD") == 0)
			http_method = METHOD_HEAD;
		debug("     ... http_method=%d\n", http_method);
		for (http_option_line = NULL, n = 0;
		     getline(&http_option_line, &n, client_stream) >= 0 && strcmp(http_option_line, "\r\n") != 0;
		     free(http_option_line), http_option_line = NULL, n = 0) {
			debug("http_option_line: %s", http_option_line);
			if (!MethodIsConditional(http_method)) {

				/*** parse option line, recognize "If-Modified-Since" option,
				 *** and possibly add METHOD_CONDITIONAL flag to http_method
				 *** (and set since_tm by using strptime)
				 ***/
/*** TO BE DONE 2.2 START ***/
                  strtokr_save=strtok(http_request_line,":");
                 if(strcmp(http_request_line,"If-Modified-Since")==0)
                  {
                        http_method=METHOD_CONDITIONAL;
                        /*switcho tra i vari formati compatibili? o uso solo quello consigliato?*/
                        
                        /*chiola più in su usava per la strftime "%a, %d %b %Y %T GMT"*/
                        
                        if(!strptime(strtokr_save,"%a, %0d %b %y %0H:%0M:%0S", &since_tm))/*formato consigliato*/
                        {
                            /*messaggio di errore*/
                        }
                        /*se c'è del garbage dopo cosa facc?*/
                  }
                  else{/*?BAD REQUEST?*/}
                 
                  
               
/*** TO BE DONE 2.2 END ***/

			}
		}
		free(http_option_line);
		if (http_method == METHOD_NONE) {
			printf("method not implemented\n");
			SEND_RESPONSE(client_fd, 501,
#ifdef INCaPACHE_2_3
				      1, connection_no, thread_idx,
#endif
				      method_str, NULL);
			break;
		}
		printf("\n-----------------------------------------------\n");
		if (strcmp(filename, "/") == 0)
			filename = "index.html";
		else
			filename += 1; /* remove leading '/' */
		debug("http_method=%d, filename=%s\n", http_method, filename);
		stat_p = my_malloc(sizeof(*stat_p));
		if (access(filename, R_OK) != 0 || stat(filename, stat_p) < 0) {
			debug("    ... file %s not found!\n", filename);
			free(stat_p);
			SEND_RESPONSE(client_fd, RESPONSE_CODE_NOT_FOUND,
#ifdef INCaPACHE_2_3
				      is_http1_0, connection_no, thread_idx,
#endif
				      filename, NULL);
		} else {
			if (MethodIsConditional(http_method)) {

				/*** compare file last modification time and decide
				 *** whether to transform http_method to METHOD_NOT_CHANGED
				 *** Use timegm() to convert from struct tm to time_t
				 ***/
/*** TO BE DONE 2.2 START ***/
             /*time gm non considera più le var sotto  i secondi*/
             if(timegm(&since_tm)<(stat_p->st_mtime))
             {http_method=METHOD_GET;}
             else{http_method=METHOD_NOT_CHANGED;}
            
/*** TO BE DONE 2.2 END ***/

			}
			switch (http_method) {
			case METHOD_HEAD :
				debug("    ... sending header for file %s\n", filename);
				free(stat_p);
				SEND_RESPONSE(client_fd, RESPONSE_CODE_OK, /*** OK, without body ***/
#ifdef INCaPACHE_2_3
					      is_http1_0, connection_no, thread_idx,
#endif
					      filename, NULL);
				break;
			case METHOD_NOT_CHANGED :
				debug("    ... file %s not modified\n", filename);
				free(stat_p);
				SEND_RESPONSE(client_fd, RESPONSE_CODE_NOT_MODIFIED, /*** Not Modified, without body ***/
#ifdef INCaPACHE_2_3
					      is_http1_0, connection_no, thread_idx,
#endif
					      NULL, NULL);
				break;
			case METHOD_GET :
				debug("    ... sending file %s\n", filename);
				SEND_RESPONSE(client_fd, RESPONSE_CODE_OK, /*** OK, with body ***/
#ifdef INCaPACHE_2_3
					      is_http1_0, connection_no, thread_idx,
#endif
					      filename, stat_p);
				break;
			default:
				assert(0);
			}
		}
#ifdef INCaPACHE_2_3
		if (is_http1_0)
#endif
			break;
	}
#ifdef INCaPACHE_2_3
	join_all_threads(connection_no);
#endif	
	if (close(client_fd))
		fail_errno("Cannot close the connection");
}

