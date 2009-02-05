#include <gst/gst.h>

static GMainLoop *loop;
static GstElement *source, *filter, *sink;

gint
main (gint   argc,
      gchar *argv[])
{
  GstElement *pipeline;
  pthread_t threadid;
  void *exit_status;
	int portno = 6001;
	if (argc >= 2) {
		portno = atoi(argv[1]);
	}

	/* init */
	gst_init (&argc, &argv);

	/* create pipeline, add handler */
	pipeline = gst_pipeline_new ("my_pipeline");

	// create elements
	source = gst_element_factory_make("udpsrc", "source");
	filter = gst_element_factory_make("audioparse", "filter");
	sink = gst_element_factory_make("autoaudiosink", "sink");
	if (!source || !filter || !sink) {
		if (!source) g_print ("[E] failed to create udpsrc element\n");
		if (!filter) g_print ("[E] failed to create audioparse element\n");
		if (!sink) g_print ("[E] failed to create autoaudiosink element\n");
    		return -1;
	}
	g_object_set(G_OBJECT(source), "port", portno, NULL);

	// add to pipeline before linking
	gst_bin_add_many(GST_BIN(pipeline), source, filter, sink, NULL);
	// link
	if (!gst_element_link_many(source, filter, sink, NULL)) {
		g_print("[E] failed to link elements");
		return -1;
	}
	// start playing
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);

	g_print("[I] starting gstreamer main loop\n");
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  /* clean up */
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  g_main_loop_unref (loop);

  return 0;
}
