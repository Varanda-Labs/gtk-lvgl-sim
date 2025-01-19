
#include <gtk/gtk.h>
#include "sim_config.h"
#include "lvgl-integration.h"
#include "logger.h"

#define APP_PREFIX "/com/varandalabs/lvglsim"


static gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    if (lv_int_surface != NULL)
        cairo_surface_destroy (lv_int_surface);

    gtk_widget_set_size_request( widget, LCD_WIDTH, LCD_HEIGHT);

    lv_int_surface = gdk_window_create_similar_surface (
                                               gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               LCD_WIDTH,   // maybe:  gtk_widget_get_allocated_width (widget)
                                               LCD_HEIGHT); // maybe: gtk_widget_get_allocated_width (widget)

    return TRUE;
}

static gboolean cb_draw (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    CHECK_SURFACE_RET();
    cairo_set_source_surface (cr, lv_int_surface, 0, 0);
    cairo_paint (cr);

    return TRUE;
}

static gboolean cb_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
     if (lv_int_surface == NULL)
        return FALSE;

    if (event->button == GDK_BUTTON_PRIMARY) {
        lv_int_set_pointer(event->x, event->y, 2 * TOUCH_PRESSURE);
    }

    return TRUE;
}

static gboolean cb_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
     if (lv_int_surface == NULL)
        return FALSE;

    if (event->button == GDK_BUTTON_PRIMARY) {
        lv_int_set_pointer(event->x, event->y, 0);
    }

    return TRUE;
}

static gboolean cb_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    if (lv_int_surface == NULL)
        return FALSE;

    if (event->state & GDK_BUTTON1_MASK) {
        lv_int_set_pointer(event->x, event->y, 2 * TOUCH_PRESSURE);
    }

    return TRUE;
}

static gboolean cb_leave_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    if (lv_int_surface == NULL)
        return FALSE;

    lv_int_pointer_left();

    return TRUE;
}

static void cb_window_destory ()
{
    if (lv_int_surface != NULL)
        cairo_surface_destroy (lv_int_surface);
    drawing_area = NULL;
    
}


static gboolean time_handler(GtkWidget *widget) {
    if (widget == NULL) {
        return FALSE;
    }
    lv_int_run_slice();
    return TRUE;
}

static void cb_application_activate (GtkApplication* app, gpointer user_data)
{
    GtkBuilder* builder = gtk_builder_new ();
    g_return_if_fail (builder != NULL);

    GError* error = NULL;
    builder = gtk_builder_new_from_resource(APP_PREFIX "/lvglsim.glade");
    if (! builder) {
    //if (!gtk_builder_add_from_file (builder, "src/sim/lvgl-sim.glade", &error)) {
        if (error) {
            g_error ("Failed to load: %s", error->message);
            g_error_free (error);
            return ;
        }
    }

    GtkApplicationWindow *window = GTK_APPLICATION_WINDOW (gtk_builder_get_object (builder, "application_window"));
    g_warn_if_fail (window != NULL);
    g_object_set (window, "application", app, NULL);
    gtk_window_set_title (GTK_WINDOW (window), "LVGL Simulator");
    g_signal_connect (window, "destroy", G_CALLBACK (cb_window_destory), NULL);

    {
        drawing_area = GTK_WIDGET (gtk_builder_get_object (builder, "drawing_area"));
        g_warn_if_fail (drawing_area != NULL);

        g_signal_connect (drawing_area, "draw", G_CALLBACK (cb_draw), NULL);
        g_signal_connect (drawing_area, "configure-event", G_CALLBACK (configure_event_cb), NULL);
        g_signal_connect (drawing_area, "motion-notify-event", G_CALLBACK (cb_motion_notify_event), NULL);
        g_signal_connect (drawing_area, "button-press-event", G_CALLBACK (cb_button_press_event), NULL);
        g_signal_connect (drawing_area, "button-release-event", G_CALLBACK (cb_button_release_event), NULL);
        g_signal_connect (drawing_area, "leave_notify_event", G_CALLBACK (cb_leave_event), NULL);

        gtk_widget_set_events (drawing_area, gtk_widget_get_events (drawing_area)
                                           | GDK_BUTTON_PRESS_MASK
                                           | GDK_BUTTON_RELEASE_MASK
                                           | GDK_LEAVE_NOTIFY_MASK
                                           | GDK_POINTER_MOTION_MASK);
    }

    gtk_widget_show_all (GTK_WIDGET (window));
    g_timeout_add(LVGL_PERIOD_TIME, (GSourceFunc) time_handler, (gpointer) window);
    g_object_unref (builder);
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.varandalabs.mv", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (cb_application_activate), NULL);

    lv_int_init();
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
