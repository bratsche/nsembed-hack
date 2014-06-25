#include "gtkembedcontainer.h"

static void gtk_embed_container_realize       (GtkWidget        *widget);
static void gtk_embed_container_size_request  (GtkWidget        *widget,
                                               GtkRequisition   *requisition);
static void gtk_embed_container_size_allocate (GtkWidget        *widget,
                                               GtkAllocation    *allocation);
static void gtk_embed_container_add           (GtkContainer     *container,
                                               GtkWidget        *widget);
static void gtk_embed_container_remove        (GtkContainer     *container,
                                               GtkWidget        *widget);
static void gtk_embed_container_forall        (GtkContainer     *container,
                                               gboolean          include_internals,
                                               GtkCallback       callback,
                                               gpointer          callback_data);
static GType gtk_embed_container_child_type   (GtkContainer     *container);

G_DEFINE_TYPE (GtkEmbedContainer, gtk_embed_container, GTK_TYPE_WINDOW)

enum {
  PROP_0,
  PROP_NSVIEW,
  PROP_NSWINDOW,
  PROP_HOSTWINDOW
};

GtkWidget*
gtk_embed_container_new (NSView *nsview, NSWindow *nswindow, GdkWindow *hostwindow)
{
  return g_object_new (GTK_TYPE_EMBED_CONTAINER,
                       "nsview", nsview,
                       "nswindow", nswindow,
                       "hostwindow", hostwindow,
                       NULL);
}

static void
gtk_embed_container_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GtkEmbedContainer *container = GTK_EMBED_CONTAINER (object);

  switch (prop_id)
    {
      case PROP_NSVIEW:
        container->nsview = g_value_get_pointer (value);
        break;

      case PROP_NSWINDOW:
        container->nswindow = g_value_get_pointer (value);
        break;

      case PROP_HOSTWINDOW:
        container->toplevel_host = g_value_get_object (value);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtk_embed_container_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GtkEmbedContainer *container = GTK_EMBED_CONTAINER (object);

  switch (prop_id)
    {
      case PROP_NSVIEW:
        g_value_set_pointer (value, container->nsview);
        break;

      case PROP_NSWINDOW:
        g_value_set_pointer (value, container->nswindow);
        break;

      case PROP_HOSTWINDOW:
        g_value_set_object (value, container->toplevel_host);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static gint
gtk_embed_container_button_press_event (GtkWidget *widget, GdkEventButton *event)
{
  g_print ("press!\n");
  return FALSE;
}

static gint
gtk_embed_container_button_release_event (GtkWidget *widget, GdkEventButton *event)
{
  g_print ("release!\n");
  return FALSE;
}

static gint
gtk_embed_container_event (GtkWidget *widget, GdkEvent *event)
{
  //g_print ("Event!\n");

  return FALSE;
}

static void
gtk_embed_container_class_init (GtkEmbedContainerClass *class)
{
  GObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  object_class = (GObjectClass *) class;
  widget_class = (GtkWidgetClass*) class;
  container_class = (GtkContainerClass*) class;

  object_class->set_property  = gtk_embed_container_set_property;
  object_class->get_property  = gtk_embed_container_get_property;

  widget_class->button_press_event = gtk_embed_container_button_press_event;
  widget_class->button_release_event = gtk_embed_container_button_release_event;
  widget_class->event         = gtk_embed_container_event;
  widget_class->realize       = gtk_embed_container_realize;
  // widget_class->show          = gtk_embed_container_show;
  // widget_class->hide          = gtk_embed_container_hide;
  //widget_class->size_request  = gtk_embed_container_size_request;
  widget_class->size_allocate = gtk_embed_container_size_allocate;

  g_object_class_install_property (object_class,
                                   PROP_NSVIEW,
                                   g_param_spec_pointer ("nsview",
                                                         "NSView parent",
                                                         "NSView parent",
                                                         G_PARAM_READWRITE | G_PARAM_STATIC_NAME |
                                                         G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_class_install_property (object_class,
                                   PROP_NSWINDOW,
                                   g_param_spec_pointer ("nswindow",
                                                         "NSWindow toplevel",
                                                         "NSWindow toplevel",
                                                         G_PARAM_READWRITE | G_PARAM_STATIC_NAME |
                                                         G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_class_install_property (object_class,
                                   PROP_HOSTWINDOW,
                                   g_param_spec_object ("hostwindow",
                                                        "Gdk host toplevel",
                                                        "Gdk host toplevel",
                                                        GDK_TYPE_WINDOW,
                                                        G_PARAM_READWRITE | G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));
}

static void
gtk_embed_container_init (GtkEmbedContainer *embed_container)
{
  embed_container->nsview = NULL;
}

static void
gtk_embed_container_realize (GtkWidget *widget)
{
  GtkEmbedContainer *embed = GTK_EMBED_CONTAINER (widget);
  GtkBin *bin = GTK_BIN (widget);
  gint border_width = gtk_container_get_border_width (GTK_CONTAINER (widget));
  GdkWindowAttr attributes;
  gint attributes_mask;

  g_return_if_fail (embed->nsview != NULL);

#if 0
  if (widget->allocation.x == -1 &&
      widget->allocation.y == -1 &&
      widget->allocation.width == 1 &&
      widget->allocation.height == 1)
    {
      GtkRequisition requisition;
      NSRect rect = [embed->nsview frame];
      GtkAllocation allocation = { rect.origin.x, rect.origin.y, rect.size.width, rect.size.height };

      gtk_widget_size_request (widget, &requisition);

      if (requisition.width || requisition.height)
        {
          gtk_widget_size_allocate (widget, &allocation);

          g_return_if_fail (!gtk_widget_get_realized (widget));
        }
    }
#endif

  gtk_widget_set_realized (widget, TRUE);

  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.window_type = GDK_WINDOW_NSEMBED;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new_with_nsview (gtk_widget_get_parent_window (widget),
                                               embed->nsview,
//                                               embed->nswindow,
                                               embed->toplevel_host,
                                               &attributes,
                                               attributes_mask);
  g_print (" *** widget->window == %p\n", widget->window);

  gdk_window_set_user_data (widget->window, widget);

  if (bin->child)
    gtk_widget_set_parent_window (bin->child, widget->window);

  widget->style = gtk_style_attach (widget->style, widget->window);

  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
}

static void
gtk_embed_container_size_request (GtkWidget      *widget,
                                  GtkRequisition *requisition)
{
  GtkBin *bin = GTK_BIN (widget);
  GtkEmbedContainer *embed_container = GTK_EMBED_CONTAINER (widget);
  GtkRequisition child_requisition;
  gint border_width = gtk_container_get_border_width (GTK_CONTAINER (widget));
  gint default_width, default_height;

  requisition->width = border_width * 2;
  requisition->height = border_width * 2;

  if (bin->child && gtk_widget_get_visible (bin->child))
    {
      GtkRequisition child_requisition;

      gtk_widget_size_request (bin->child, &child_requisition);

      requisition->width += child_requisition.width;
      requisition->height += child_requisition.height;
    }

    gtk_window_get_default_size (GTK_WINDOW (widget), &default_width, &default_height);

    if (default_width > 0)
      requisition->width = default_width;

    if (default_height > 0)
      requisition->height = default_height;

  NSView *nsview = embed_container->nsview;
  NSRect rect = [nsview frame];
  requisition->width = rect.size.width;
  requisition->height = rect.size.height;
  // requisition->width = MAX(requisition->width, rect.size.width);
  // requisition->height = MAX(requisition->)
}

static void
gtk_embed_container_size_allocate (GtkWidget     *widget,
                                   GtkAllocation *allocation)
{
  GtkBin *bin = GTK_BIN (widget);
  GtkEmbedContainer *embed_container = GTK_EMBED_CONTAINER (widget);
  GtkWidget *child;
  GtkRequisition child_requisition;
  GList *children;
  gint border_width;

  NSRect rect = [embed_container->nsview frame];
  allocation->width = rect.size.width;
  allocation->height = rect.size.height;

  widget->allocation = *allocation;

  //g_print ("size_allocate %d,%d\n", allocation->width, allocation->height);
      
  border_width = gtk_container_get_border_width (GTK_CONTAINER (widget));

  if (gtk_widget_get_realized (widget))
    {
      gdk_window_move_resize (widget->window,
                              allocation->x,
                              allocation->y,
                              allocation->width,
                              allocation->height);
    }

  if (bin->child && gtk_widget_get_visible (bin->child))
    {
      GtkAllocation child_allocation;

      child_allocation.x = child_allocation.y = border_width;
      child_allocation.width = allocation->width - 2 * border_width;
      child_allocation.height = allocation->height - 2 * border_width;

      gtk_widget_size_allocate (bin->child, &child_allocation);
    }
}
