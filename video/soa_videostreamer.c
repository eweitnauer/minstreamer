/**
 * soastreamer-video
 * Part of soastreamer, an udp video and audio streaming tool,
 * controlled via network.
 *
 * Copyright (C) 2009 Erik Weitnauer
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * Please see the GNU General Public License at <http://www.gnu.org/licenses/>.
 *
 * Description:
 * With this program, the data from a connected usb camera can be streamed
 * over network to a list of clients. The video streaming is done with
 * libgstreamer, using udp.
 * After start up, the program will start listening on a port and via udp, any
 * of the following string commands can be sent to it, to make changes to the
 * list of clients the video signal is streamed to:
 * ======
 * add				 <hostname:port>
 * remove			 <hostname:port>
 * set_clients <hostname:port,hostname:port,...>
 * clear    	 -
 * print			 -
 * exit				 -
 * ======
 */
 
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <gst/gst.h>

static GMainLoop *loop;
static GstElement *source, *filter, *filter2, *sink;

void error(const gchar *msg)
{
    perror(msg);
    exit(1);
}

/**
 * Tries to parse an "host:port" string. If successful, it
 * returns true. Gracefully handles NULL / empty string.
 */
gboolean parse_addr(gchar *str, gchar **host, gint *port) {
	if ((str == NULL) || (str == "")) return FALSE;
	gchar **addr = g_strsplit(str,":",2);
	if (addr[0] != NULL) *host = g_strdup(addr[0]);
	if (addr[1] != NULL) *port = atoi(addr[1]);
	g_strfreev(addr);
	return (addr[1] != NULL);
}

/**
 * Parses the passed GString message and executes the commands.
 * Returns false if the program should exit.
 */
gboolean parse_message(gchar *gstr) {
  if (gstr == NULL) return TRUE;
	// strip whitespaces at beginning and end of string
	gchar *str = g_strstrip(gstr);
	if (str == "") return TRUE;
	g_print("[D] received: '%s'\n", str);
	// split by ' '
	gchar **parts = g_strsplit(str," ",2);
	if (parts[0] == NULL) return TRUE;
	gchar *host;
	gint port;
	// check for known commands
	if (g_str_has_prefix(parts[0], "add")) {
		if (parse_addr(parts[1], &host, &port)) {
		  g_signal_emit_by_name(G_OBJECT(sink), "add", host, port, NULL);
   	  g_print("[I] addding client '%s:%i'\n",host,port);
		  g_free(host);
		}
	}
	else if (g_str_has_prefix(parts[0], "remove")) {
		if (parse_addr(parts[1], &host, &port)) {
		  g_signal_emit_by_name(G_OBJECT(sink), "remove", host, port, NULL);
   	  g_print("[I] removing client '%s:%i'\n",host,port);
 		  g_free(host);
		}
	}
	else if (g_str_has_prefix(parts[0], "clear")) {
    g_signal_emit_by_name(G_OBJECT(sink), "clear", NULL);
 	  g_print("[I] clearing client list\n");
  }
  else if (g_str_has_prefix(parts[0], "set_clients")) {
  	if (parts[1] != NULL) {
  	  g_object_set(G_OBJECT(sink), "clients", parts[1], NULL);
  	  g_print("[I] setting client list to '%s'\n",parts[1]);
  	}
  }
  else if (g_str_has_prefix(parts[0], "print")) {
  	gchar *list;
		g_object_get(G_OBJECT(sink), "clients", &list, NULL);
		g_print("[D] current client list: '%s'\n",list);
		g_free(list);
  }
  else if (g_str_has_prefix(parts[0], "exit")) {
 	  g_print("[I] received exit command, exiting...\n");
	  g_main_loop_quit(loop);
	  return FALSE;
 	}
 	else {
 		g_print("[W] command '%s' could not be parsed!\n",str);
 	}
 	return TRUE;
}

int udp_listen(int portno)
{
	   int sockfd, newsockfd, clilen;
     gchar buffer[256];
     struct sockaddr_in serv_addr;
		 int n;
     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((gchar *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     while (1) {
     		bzero(buffer,256);
     		n = recv(sockfd, buffer, 255, 0);
     		if (n < 0) error("ERROR reading from socket");
     		if (!parse_message(buffer)) break;
     }
     return 0; 
}

void *run_upd_listen(void* arg) {
  int *portno = (int *)arg; 
  g_print("[I] udp server thread running, listening on port number %i\n", *portno);
	udp_listen(*portno);
	return NULL;
}

gint
main (gint   argc,
      gchar *argv[])
{
  GstElement *pipeline;
  pthread_t threadid;
  void *exit_status;

  if (argc < 2) {
     g_print ("[E] please provide a port number\n");
     return -1;
  }
  int portno = atoi(argv[1]);

  /* init */
  gst_init (&argc, &argv);

  /* create pipeline, add handler */
  pipeline = gst_pipeline_new ("my_pipeline");

	// create elements
  source = gst_element_factory_make("v4l2src", "source");
	filter = gst_element_factory_make("ffmpegcolorspace", "filter");
	filter2 = gst_element_factory_make("jpegenc", "filter2");
	sink = gst_element_factory_make("multiudpsink", "sink");
	if (!source || !filter || !filter2 || !sink) {
    if (!source) g_print ("[E] failed to create v4l2src element\n");
    if (!filter) g_print ("[E] failed to create ffmpegcolorspace element\n");
    if (!filter2) g_print ("[E] failed to create jpegenc element\n");
    if (!sink) g_print ("[E] failed to create multiudpsink element\n");
    return -1;
  }

	// add to pipeline before linking
	gst_bin_add_many(GST_BIN(pipeline), source, filter, filter2, sink, NULL);
	
	GstCaps *caps = gst_caps_new_simple("video/x-raw-yuv",
                                      "width", G_TYPE_INT, 640,
                                      "height", G_TYPE_INT, 480,
                                      "framerate", GST_TYPE_FRACTION, 30, 1,
                                      NULL);
	// link
	if (!gst_element_link_filtered(source, filter, caps) ||
	    !gst_element_link(filter, filter2) ||
 	    !gst_element_link(filter2, sink)) {
		g_print("[E] failed to link elements");
		return -1;
	}
	// start playing
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);

	// do this inside a thread:
	g_print("[I] starting udp server thread\n");
	pthread_create(&threadid, NULL, run_upd_listen, &portno);

	g_print("[I] starting gstreamer main loop\n");
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  /* clean up */
  pthread_join(threadid, &exit_status);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  g_main_loop_unref (loop);

  return 0;
}
