#if 1

#include <gtk/gtk.h>

static cairo_surface_t *s_cairo_surface = NULL;

static void clear_surface ()
{
    cairo_t *cr = cairo_create (s_cairo_surface);

    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    cairo_destroy (cr);
}

static void draw_brush (GtkWidget *widget, gdouble x, gdouble y)
{
    #define BRUSH_SIZE 10

    cairo_t *cr = cairo_create (s_cairo_surface);

    cairo_rectangle (cr, x - BRUSH_SIZE / 2, y - BRUSH_SIZE / 2, BRUSH_SIZE, BRUSH_SIZE);
    cairo_fill (cr);
    cairo_destroy (cr);

    gtk_widget_queue_draw_area (widget, x - BRUSH_SIZE / 2, y - BRUSH_SIZE / 2, BRUSH_SIZE, BRUSH_SIZE);
}

static gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    if (s_cairo_surface != NULL)
        cairo_surface_destroy (s_cairo_surface);

    s_cairo_surface = gdk_window_create_similar_surface (
                                               gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               gtk_widget_get_allocated_width (widget),
                                               gtk_widget_get_allocated_height (widget));

    clear_surface ();

    return TRUE;
}

static gboolean cb_draw (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_surface (cr, s_cairo_surface, 0, 0);
    cairo_paint (cr);

    return TRUE;
}

static gboolean cb_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
     if (s_cairo_surface == NULL)
        return FALSE;

    if (event->button == GDK_BUTTON_PRIMARY)
        draw_brush (widget, event->x, event->y);
    else if (event->button == GDK_BUTTON_SECONDARY) {
        clear_surface ();
        gtk_widget_queue_draw (widget);
    }

    return TRUE;
}

static gboolean cb_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    if (s_cairo_surface == NULL)
        return FALSE;

    if (event->state & GDK_BUTTON1_MASK)
        draw_brush (widget, event->x, event->y);

    return TRUE;
}

static void cb_window_destory ()
{
    if (s_cairo_surface != NULL)
        cairo_surface_destroy (s_cairo_surface);
}

static void cb_application_activate (GtkApplication* app, gpointer user_data)
{
    GtkBuilder* builder = gtk_builder_new ();
    g_return_if_fail (builder != NULL);

    GError* error = NULL;
    if (!gtk_builder_add_from_file (builder, "src/sim/lvgl-sim.glade", &error)) {
        if (error) {
            g_error ("Failed to load: %s", error->message);
            g_error_free (error);
            return ;
        }
    }

    GtkApplicationWindow *window = GTK_APPLICATION_WINDOW (gtk_builder_get_object (builder, "application_window"));
    g_warn_if_fail (window != NULL);
    g_object_set (window, "application", app, NULL);
    gtk_window_set_title (GTK_WINDOW (window), "Cairo draw test");
    g_signal_connect (window, "destroy", G_CALLBACK (cb_window_destory), NULL);

    {
        GtkWidget *drawing_area = GTK_WIDGET (gtk_builder_get_object (builder, "drawing_area"));
        g_warn_if_fail (drawing_area != NULL);

        g_signal_connect (drawing_area, "draw", G_CALLBACK (cb_draw), NULL);
        g_signal_connect (drawing_area, "configure-event", G_CALLBACK (configure_event_cb), NULL);
        g_signal_connect (drawing_area, "motion-notify-event", G_CALLBACK (cb_motion_notify_event), NULL);
        g_signal_connect (drawing_area, "button-press-event", G_CALLBACK (cb_button_press_event), NULL);

        gtk_widget_set_events (drawing_area, gtk_widget_get_events (drawing_area)
                                           | GDK_BUTTON_PRESS_MASK
                                           | GDK_POINTER_MOTION_MASK);
    }

    gtk_widget_show_all (GTK_WIDGET (window));
    g_object_unref (builder);
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk3.cairo", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (cb_application_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}



#else
/* pixel.c
 *
 * Compile: gcc -ggdb lvgl-sim.c $(pkg-config --cflags --libs gtk+-3.0) -o lvgl-sim
 * Run: ./pixel
 *
 * Author: Mohammed Sadiq <www.sadiqpk.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later OR CC0-1.0
 */

#include <gtk/gtk.h>

#define BYTES_PER_R8G8B8 3
#define WIDTH  400

static void
fill_row (GByteArray *array,
          guint8      value,
          int         row_size)
{
  guint i;

  for (i = 0; i < row_size; i++) {
    /* Fill the same values for RGB */
    g_byte_array_append (array, &value, 1); /* R */
    g_byte_array_append (array, &value, 1); /* G */
    g_byte_array_append (array, &value, 1); /* B */
  }
}

static void
add_pixel_picture (GtkPicture *picture)
{
  g_autoptr(GBytes) bytes = NULL;
  GdkTexture *texture;
  GByteArray *pixels;
  gsize height;

  /* Draw something interesting */
  pixels = g_byte_array_new ();
  for (guint i = 0; i <= 0xff ; i++)
    fill_row (pixels, i, WIDTH);

  for (guint i = 0; i <= 0xff; i++)
    fill_row (pixels, 0xff - i, WIDTH);

  height = pixels->len / (WIDTH * BYTES_PER_R8G8B8);
  bytes = g_byte_array_free_to_bytes (pixels);

  texture = gdk_memory_texture_new (WIDTH, height,
                                    GDK_MEMORY_R8G8B8,
                                    bytes,
                                    WIDTH * BYTES_PER_R8G8B8);
  gtk_picture_set_paintable (picture, GDK_PAINTABLE (texture));
}

static void
app_activated_cb (GtkApplication *app)
{
  GtkWindow *window;
  GtkWidget *picture;

  window = GTK_WINDOW (gtk_application_window_new (app));
  g_object_set (window,
                "width-request", 500,
                "height-request", 400,
                NULL);
  picture = gtk_picture_new ();
  gtk_widget_add_css_class (picture, "frame");
  g_object_set (picture,
                "margin-start", 96,
                "margin-end", 96,
                "margin-top", 96,
                "margin-bottom", 96,
                NULL);

  gtk_window_set_child (window, picture);
  add_pixel_picture (GTK_PICTURE (picture));

  gtk_window_present (window);
}

int
main (int   argc,
      char *argv[])
{
  g_autoptr(GtkApplication) app = gtk_application_new (NULL, 0);

  g_signal_connect (app, "activate", G_CALLBACK (app_activated_cb), NULL);

  lv_int_init();

  return g_application_run (G_APPLICATION (app), argc, argv);
}
#endif

