#include <gdk/gdkquartz.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtknsview.h>

#define GTK_TYPE_EMBED_CONTAINER         (gtk_embed_container_get_type ())
#define GTK_EMBED_CONTAINER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GTK_TYPE_EMBED_CONTAINER, GtkEmbedContainer))
#define GTK_EMBED_CONTAINER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), GTK_TYPE_EMBED_CONTAINER, GtkEmbedContainerClass))
#define GTK_IS_EMBED_CONTAINER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTK_TYPE_EMBED_CONTAINER))
#define GTK_IS_EMBED_CONTAINER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GTK_TYPE_EMBED_CONTAINER))
#define GTK_EMBED_CONTAINER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GTK_TYPE_EMBED_CONTAINER, GtkEmbedContainerClass))


typedef struct _GtkEmbedContainer        GtkEmbedContainer;
typedef struct _GtkEmbedContainerClass   GtkEmbedContainerClass;
typedef struct _GtkEmbedContainerChild   GtkEmbedContainerChild;

struct _GtkEmbedContainer
{
  GtkWindow parent_object;

  NSView *nsview;
  NSWindow *nswindow;
  GdkWindow *toplevel_host;
};

struct _GtkEmbedContainerClass
{
  GtkWindowClass parent_class;
};

GType      gtk_embed_container_get_type (void) G_GNUC_CONST;
GtkWidget* gtk_embed_container_new      (NSView *nsview, NSWindow *nswindow, GdkWindow *host_toplevel);
