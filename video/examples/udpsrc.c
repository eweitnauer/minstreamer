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
	//source = gst_element_factory_make("v4l2src", "source");
	source = gst_element_factory_make("udpsrc", "source");
	g_object_set(G_OBJECT(source), "port", 5001, NULL);
	filter = gst_element_factory_make("jpegdec", "filter");
	sink = gst_element_factory_make("autovideosink", "sink");
	
	// add to pipeline before linking
	gst_bin_add_many(GST_BIN(pipeline), source, filter, sink, NULL);
	
	// link
	if (!gst_element_link_many(source, filter, sink, NULL)) {
		g_warning("Failed to link elements");
	}
	// start playing
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);

  /* create a mainloop that runs/iterates the default GLib main context
   * (context NULL), in other words: makes the context check if anything
   * it watches for has happened. When a message has been posted on the
   * bus, the default main context will automatically call our
   * my_bus_callback() function to notify us of that message.
   * The main loop will be run until someone calls g_main_loop_quit()
   */
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  /* clean up */
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  g_main_loop_unref (loop);

  return 0;
}

