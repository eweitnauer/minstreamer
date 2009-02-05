#include <gst/gst.h>

int
main (int   argc,
      char *argv[])
{
  GstElement *element;
  gchar *name;
  GstElementFactory *factory;

  /* init GStreamer */
  gst_init (&argc, &argv);

  /* create element */
  element = gst_element_factory_make ("multiudpsink", "yeah");
  if (!element) {
    g_print ("Failed to create element of type 'multiudpsink'\n");
    return -1;
  }
  
  g_print ("Succeded in creating element of type 'multiudpsink'\n");
  /* get name */
  g_object_get(G_OBJECT (element), "name", &name, NULL);
  g_print ("The name of the element is '%s'.\n", name);
  g_free (name);

  gst_object_unref (GST_OBJECT (element));
  
  /* get factory */
  factory = gst_element_factory_find ("jpegenc");
  if (!factory) {
    g_print ("You don't have the 'jpegenc' element installed!\n");
    return -1;
  }

  /* display information */
  g_print ("The '%s' element is a member of the category %s.\n"
           "Description: %s\n",
           gst_plugin_feature_get_name (GST_PLUGIN_FEATURE (factory)),
           gst_element_factory_get_klass (factory),
           gst_element_factory_get_description (factory));

  return 0;
}

