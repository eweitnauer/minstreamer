#include <gst/gst.h>

static GMainLoop *loop;



gint
main (gint   argc,
      gchar *argv[])
{
  GstElement *pipeline;
  GstElement *source, *filter, *sink;

  /* init */
  gst_init (&argc, &argv);

  /* create pipeline, add handler */
  pipeline = gst_pipeline_new ("my_pipeline");

	// create elements
  source = gst_element_factory_make("v4l2src", "source");
	filter = gst_element_factory_make("jpegenc", "filter");
	sink = gst_element_factory_make("multiudpsink", "sink");
	if (!source || !filter || !sink) {
    if (!source) g_print ("Failed to create v4l2src element\n");
    if (!filter) g_print ("Failed to create jpegenc element\n");
    if (!sink) g_print ("Failed to create multiudpsink element\n");
    return -1;
  }
	//g_object_set(G_OBJECT(sink), "clients", "127.0.0.1:5000,127.0.0.1:5001", NULL);
	
	// add to pipeline before linking
	gst_bin_add_many(GST_BIN(pipeline), source, filter, sink, NULL);
	
	// link
	if (!gst_element_link_many(source, filter, sink, NULL)) {
		g_warning("Failed to link elements");
	}
	// start playing
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);

  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  /* clean up */
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  g_main_loop_unref (loop);

  return 0;
}
