#import <Cocoa/Cocoa.h>
#include <gtk/gtk.h>
#include <gdk/gdkquartz.h>

@interface NSGtkView : NSView
{
  NSTrackingRectTag trackingTag;
  GtkWidget *parent;
  GtkWidget *widget;
}

- (void)setGtkParent:(GtkWidget*)parent;
- (void)setGtkWidget:(GtkWidget*)child;

- (GdkWindow*)getGdkWindow;

@end