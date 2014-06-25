#import "NSGtkView.h"

@implementation NSGtkView

static void update_allocation (GtkWidget *widget, NSGtkView *nsview)
{
  GdkScreen *screen = gtk_widget_get_screen (widget);
  int height = gdk_screen_get_height (screen);
  GtkAllocation allocation;
  NSRect fr = [nsview frame];
  NSRect rect = [nsview convertRect:fr toView:nsview];
  NSPoint pt = [nsview convertPointFromBacking:rect.origin];

  allocation.x = pt.x;
  allocation.y = pt.y; // height - (int)pt.y - rect.size.height;
  allocation.width = rect.size.width;
  allocation.height = rect.size.height;

  //g_print ("allocation %d, %d, %d, %d\n", pt.x, pt.y, rect.size.width, rect.size.height);

  gtk_widget_size_allocate (widget, &allocation);

  //g_print ("allocation [%d,%d,%d,%d]\n", allocation.x, allocation.y, allocation.width, allocation.height);
}

- (id)initWithFrame:(NSRect)frameRect
{
  if ((self = [super initWithFrame:frameRect]) == nil)
    {
      return self;
    }

  //[self setAutoresizesSubviews:YES];

  trackingTag = [self addTrackingRect:[self bounds] owner:self userData:nil assumeInside:NO];

  return self;
}

- (void)dealloc
{
  [self removeTrackingRect:trackingTag];
  [super dealloc];
}

- (void)viewBoundsChanged:(NSNotification *)notification
{
  //g_print ("viewBoundsChanged\n");
  //update_allocation (widget, self);
}

- (void)viewWillDraw
{
  g_print ("viewWillDraw\n");
  //update_allocation (widget, self);
  NSRect rect = [self frame];
  //g_print ("%f %f\n", rect.size.width, rect.size.height);
  gtk_window_resize (GTK_WINDOW (widget), (int)rect.size.width, (int)rect.size.height);
}

- (void)drawRect:(NSRect)rect
{
  g_print ("   drawRect:\n");
}

- (void)internalMouseEvent:(NSEvent *)evt
{
  g_print ("internalMouseEvent\n");
  // GdkWindow *gdkwindow = gtk_widget_get_window (widget);
  // GdkEvent *event;
  // GdkEventType type;

  // switch ([evt type])
  //   {
  //     case NSLeftMouseDown:
  //     case NSRightMouseDown:
  //     case NSOtherMouseDown:
  //       type = GDK_BUTTON_PRESS;
  //       event = gdk_event_new (type);
  //       event->any.type = type;
  //       event->button.window = gdkWindow;
  //       event->button.time = XXX;
  //       event->x = x;
  //       event->y = y;
  //       event->x_root = x_root;
  //       event->y_root = y_root;
  //       event->button.state = 0;
  //       event->button.button = 1;

  //       break;

  //     case NSLeftMouseUp:
  //     case NSRightMouseUp:
  //     case NSOtherMouseUp:
  //       type = GDK_BUTTON_RELEASE;
  //       event = gdk_event_new (type);
  //       event->any.type = type;
  //       break;

  //     case NSLeftMouseDragged:
  //     case NSRightMouseDragged:
  //     case NSOtherMouseDragged:
  //     case NSMouseMoved:
  //       type = GDK_MOTION_NOTIFY;
  //       event = gdk_event_new (type);
  //       event->any.type = type;

  //     default:
  //       type = GDK_NOTHING;
  //   }

  // gtk_main_do_event (event);


  // NSWindow *nswindow = gdk_quartz_window_get_nswindow (gdkwindow);
  // NSView *nsview = gdk_quartz_window_get_nsview (gdkwindow);
  // NSEvent *event = [NSEvent mouseEventWithType: [evt type]
  //                                     location: [evt locationInWindow]
  //                                modifierFlags: [evt modifierFlags]
  //                                    timestamp: [[NSApp currentEvent] timestamp]
  //                                 windowNumber: [nswindow windowNumber]
  //                                      context: nil
  //                                  eventNumber: 0
  //                                   clickCount: [evt clickCount]
  //                                     pressure: 1.0];

  //[NSApp postEvent:event atStart: NO];

  //gtk_widget_queue_draw (widget);

  [self setNeedsDisplay:YES];
}

// - (void)mouseMoved:(NSEvent *)evt
// {
//   [self internalMouseEvent:evt];
// }

// - (void)mouseDown:(NSEvent *)evt
// {
//   //g_print ("mouseDown\n");
//   [self internalMouseEvent:evt];
// }

// - (void)mouseUp:(NSEvent *)evt
// {
//   //g_print ("mouseUp\n");
//   [self internalMouseEvent:evt];
// }

- (BOOL)acceptsFirstResponder
{
  return YES;
}

- (BOOL)becomeFirstResponder
{
  return YES;
}

- (void)setGtkParent:(GtkWidget*)parent_widget
{
  parent = parent_widget;
}

static void
queue_draw (GtkWidget *widget, NSGtkView *nsview)
{
  /*
  update_allocation (widget, nsview);
  gtk_widget_queue_draw (widget);
  */
}

- (void)setGtkWidget:(GtkWidget*)child_widget
{
  widget = child_widget;
  //gtk_container_add (GTK_CONTAINER (parent), child_widget);

  g_signal_connect (widget, "realize", G_CALLBACK (queue_draw), self);

  setNeedsDisplay:YES;
}

- (BOOL)isOpaque
{
  return NO;
}

- (GdkWindow*)getGdkWindow
{
  if (widget == NULL)
    return NULL;

  return gtk_widget_get_window (widget);
}

@end
