#import <Cocoa/Cocoa.h>
#include <gtk/gtk.h>
#include <gdk/gdkquartz.h>
#include "gtkembedcontainer.h"
#import "NSGtkView.h"

static void
print_window (GdkWindow *window, int depth)
{
  GList *list = NULL;

  g_print ("%*s" "%p\n", (depth + 1) * 2, " ", window);

  for (list = gdk_window_get_children (window); list; list = list->next)
    {
      GdkWindow *child = (GdkWindow *)list->data;

      print_window (child, depth + 1);
    }
}

static gboolean
timer_callback (gpointer data)
{
  GtkWidget *widget = (GtkWidget *)data;
  GdkWindow *window = gtk_widget_get_window (widget);

  g_print (" === Main window hierarchy ===\n");
  print_window (gdk_window_get_toplevel (window), 0);
  g_print (" === (end of main window ) ===\n");

  return FALSE;
}

static gboolean timer_callback2 (gpointer data)
{
  GtkWidget *widget = (GtkWidget *)data;
  GdkWindow *window = gtk_widget_get_window (widget);

  g_print (" === Embed window hierarchy ===\n");
  print_window (gdk_window_get_toplevel (window), 0);
  g_print (" === (end of embed window ) ===\n");

  return FALSE;
}

static void
button_clicked_cb (GtkWidget *widget, gpointer data)
{
  g_print ("Button was just clicked!\n");
}

int
main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *gtknsview;
  GdkWindow *gdkwindow;
  NSView *nsview;
  NSWindow *nswindow;

  gtk_init (&argc, &argv);

  // Create top-level GtkWindow
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_show (window);
  gdkwindow = gtk_widget_get_window (window);
  g_print (" -=-==-=-= GtkWindow's gdkwindow is %p\n", gdkwindow);
  nswindow = gdk_quartz_window_get_nswindow (gdkwindow);

  NSSplitView *nssplit = [[NSSplitView alloc] initWithFrame:nswindow.frame];
  NSButton *nsbutton = [[NSButton alloc] initWithFrame:nssplit.frame];
  [nssplit addSubview:nsbutton];

  // Create a GtkNSView. This is our NSView top-level
  gtknsview = gtk_ns_view_new (nssplit);
  gtk_container_add (GTK_CONTAINER (window), gtknsview);

  GtkWidget *bar = gtk_button_new_with_label ("gtk button");
  g_signal_connect (bar, "clicked", G_CALLBACK (button_clicked_cb), NULL);
  gtk_widget_show (bar);

  /*
  GtkWidget *embed = gtk_embed_container_new ();
  [foo setGtkParent:embed];
  [foo setGtkWidget:bar];
  gtk_embed_container_set_gtknsview (GTK_EMBED_CONTAINER (embed),
                                     GTK_NS_VIEW (gtknsview));
  */
  NSGtkView *nsgtk = [[NSGtkView alloc] initWithFrame:nssplit.frame];
  [nssplit addSubview:nsgtk];
  [nssplit setAutoresizesSubviews:YES];

  GtkWidget *embed = gtk_embed_container_new (nsgtk, nswindow, gdkwindow); // gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_register_embedded (GTK_WINDOW (window), embed);
  [nsgtk setGtkWidget:embed];
  gtk_container_add (GTK_CONTAINER (embed), bar);
  gtk_widget_show_all (embed);
  // gtk_widget_realize (embed);
  // GdkWindow *gdkwin = gtk_widget_get_window (embed);
  // g_object_set_data (G_OBJECT (gdkwin), "nsparentview", nsgtk);

  // gtk_widget_set_parent_window (bar, gdkwin);

  // Add the embed container to the window
  gtk_widget_show_all (window);

  g_timeout_add (100, timer_callback, window);
  g_timeout_add (150, timer_callback2, embed);

  gtk_main ();

  return 0;
}
