/*
 *************************************
 *
 *  GTK LVGL Simulator
 *
 *  Copyright (c) 2025 Varanda Labs
 *  MIT License
 *
 *************************************
 */

#include <gtk/gtk.h>
#include "sim_config.h"
#include "lvgl-integration.h"
#include "logger.h"
#include "app-ui.h"
#include "license.h"

#define APP_PREFIX "/com/varandalabs/lvglsim"
#define WINDOW_SIDE_BY_SIDE_SEPARATION   5
#define LOGO_DIVIDER 10

#define VERSION "0.01"

#define GREETINGS "LVGL Simulator version " VERSION " - by Varanda Labs Inc.\n\n"
#define CMD_OPTIONS "Options:\n  --show-gpio  show GPIO dialog at start up.\n\n"

static GdkPixbuf * led_off_pixbuf = NULL;
static GdkPixbuf * led_on_pixbuf = NULL;
static GdkPixbuf * logo_pixbuf = NULL;
static GtkWidget * leds[4];

static GtkWindow * gpio_win = NULL;
static GtkWindow * about_win = NULL;
static GtkApplication *app = NULL;

static int show_gpio_at_start = 0;

void set_led(int index, int state)
{
    gtk_image_set_from_pixbuf (leds[index], (state) ? led_on_pixbuf : led_off_pixbuf);
}

void on_gpio_close_clicked(GtkWidget *widget, gpointer data)
{
    // LOG("Close button\n");
    gtk_widget_hide(gpio_win);
}

void on_menu_quit(GtkWidget *widget, gpointer data)
{
    // LOG("Close button, data = %p\n", data);
    g_application_quit(G_APPLICATION(app));
}

void on_menu_about(GtkWidget *widget, gpointer data)
{
    LOG("about, data = %p\n", data);
    if (about_win) {
        gtk_widget_show_all(about_win);
    }
}

static gboolean on_about_win_delete(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_widget_hide(widget);
    return TRUE;
}

void on_about_dialog_close_button(GtkWidget *widget, gpointer data)
{
    LOG("About button\n");
    gtk_widget_hide(about_win);
}

void on_menu_gpio(GtkWidget *widget, gpointer data)
{
    // LOG("Menu GPIO, data = %p\n", data);
    if (gpio_win) {
        gtk_widget_show_all (GTK_WIDGET (gpio_win));
    }

}

void on_slider(GtkWidget *widget, gpointer data)
{
    gdouble v = gtk_range_get_value (widget);
    // LOG("Slider, data = %f\n", v);
    ui_set_bar(v);
}

void on_sw_set(GtkWidget *widget, gpointer data)
{
    char * name = gtk_widget_get_name(widget);
    // LOG("Switch %s is %s\n", name, (data) ? "ON" : "OFF");
    int i;

    if (strcmp(name, "sw1") == 0) {
        i = 0;
    }
    else if (strcmp(name, "sw2") == 0) {
        i = 1;
    }
    else if (strcmp(name, "sw3") == 0) {
        i = 2;
    }
    else if (strcmp(name, "sw4") == 0) {
        i = 3;
    }
    else  {
        LOG_E("error: unknown widget name %s\n", name);
        return;
    }

    handle_gpio_input_state(i, (int) data);
}


static gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    if (lv_int_surface != NULL)
        cairo_surface_destroy (lv_int_surface);

    gtk_widget_set_size_request( widget, LCD_WIDTH, LCD_HEIGHT);

    lv_int_surface = gdk_window_create_similar_surface (
                                               gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               LCD_WIDTH,
                                               LCD_HEIGHT);

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

// place main window and gpio side-by-side in the center of the screen
static void set_windows_positions(GtkWindow * main_win, GtkWindow * gpio_win)
{
    GdkRectangle workarea = {0};
    gdk_monitor_get_workarea(   gdk_display_get_primary_monitor(gdk_display_get_default()),
                                &workarea);

    gint main_width, main_height;
    gint gpio_width, gpio_height;
    gint total_width, max_height;
    gint main_x, main_y;      // final pos of the main window

    gtk_window_get_size (main_win, &main_width, &main_height);
    gtk_window_get_size (gpio_win, &gpio_width, &gpio_height);
    total_width = main_width + gpio_width;
    max_height = MAX(main_height, gpio_height);
    main_x = (workarea.width - total_width) / 2; // we want to center both side-by-side
    main_y = (workarea.height - max_height) / 2;

    gtk_window_move (main_win, main_x, main_y);
    gtk_window_move (gpio_win, main_x + main_width + WINDOW_SIDE_BY_SIDE_SEPARATION, main_y);
}

static gboolean on_gpio_win_delete(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_widget_hide(widget);
    return TRUE;
}
static     GtkApplicationWindow *app_win = NULL;
static void cb_application_activate (GtkApplication* app, gpointer user_data)
{
    GtkBuilder* builder = gtk_builder_new ();
    g_return_if_fail (builder != NULL);

    GError* error = NULL;
    builder = gtk_builder_new_from_resource(APP_PREFIX "/lvglsim.glade");
    if (! builder) {
        if (error) {
            g_error ("Failed to load: %s\n", error->message);
            g_error_free (error);
            return ;
        }
    }

    gpio_win = GTK_WINDOW (gtk_builder_get_object (builder, "id_gpio_dialog"));
    about_win = GTK_WINDOW (gtk_builder_get_object (builder, "id_about_dialog"));

    app_win = GTK_APPLICATION_WINDOW (gtk_builder_get_object (builder, "application_window"));
    g_warn_if_fail (app_win != NULL);
    g_object_set (app_win, "application", app, NULL);
    gtk_window_set_title (GTK_WINDOW (app_win), "LVGL Simulator");

    GdkPixbuf * icon = gdk_pixbuf_new_from_resource (APP_PREFIX "/lvgl_icon.png", &error);
    if (icon) {
        gtk_window_set_icon(GTK_WINDOW(app_win), icon);
    }
    else {
        LOG_E("Could not load icon\n");
    }

    led_on_pixbuf = gdk_pixbuf_new_from_resource (APP_PREFIX "/led_on.png", &error);
    led_off_pixbuf = gdk_pixbuf_new_from_resource (APP_PREFIX "/led_off.png", &error);
    if (led_on_pixbuf == 0 || led_off_pixbuf == 0) {
        LOG_E("Could not load LED images\n");
    }

    leds[0] = gtk_builder_get_object (builder, "id_led_1");
    leds[1] = gtk_builder_get_object (builder, "id_led_2");
    leds[2] = gtk_builder_get_object (builder, "id_led_3");
    leds[3] = gtk_builder_get_object (builder, "id_led_4");

    g_signal_connect (app_win, "destroy", G_CALLBACK (cb_window_destory), NULL);

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

    gtk_builder_connect_signals(builder,NULL);

    gtk_widget_show_all (GTK_WIDGET (app_win));


    if (gpio_win) {
        set_windows_positions(app_win, gpio_win);
        g_signal_connect(G_OBJECT(gpio_win), 
            "delete-event", G_CALLBACK(on_gpio_win_delete), NULL);

        if (show_gpio_at_start) {
            gtk_widget_show_all (GTK_WIDGET (gpio_win));
        }
    }

    if (about_win)
    {
        GdkPixbuf * pixbuf = gdk_pixbuf_new_from_resource (APP_PREFIX "/logo.png", &error);
        logo_pixbuf = gdk_pixbuf_scale_simple ( pixbuf,    // const GdkPixbuf* src,
                                                1688 / LOGO_DIVIDER, // int dest_width, // original: 1688 x 180
                                                180 / LOGO_DIVIDER,  //  int dest_height,
                                                GDK_INTERP_BILINEAR); // GdkInterpType interp_type

        gtk_about_dialog_set_logo (  (GtkAboutDialog*) about_win,   logo_pixbuf);
        gtk_about_dialog_set_license( (GtkAboutDialog*) about_win, license);
        gtk_about_dialog_set_version ( (GtkAboutDialog*) about_win, "version: " VERSION);

        g_signal_connect(G_OBJECT(about_win), 
            "delete-event", G_CALLBACK(on_about_win_delete), NULL);
    }

    g_timeout_add(LVGL_PERIOD_TIME, (GSourceFunc) time_handler, (gpointer) app_win);
    g_object_unref (builder);
}

int main (int argc, char **argv)
{
    int status;

    printf(GREETINGS);
    // small argument interpreter
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) {
            printf(CMD_OPTIONS);
            return 1;
        }
        else if (strcmp(argv[1], "--show-gpio") == 0) {
            show_gpio_at_start = 1;
        }
        else {
            printf("Unknown parameter: %s\n", argv[1]);
            return 1;
        }
    }

    app = gtk_application_new ("org.varandalabs.mv", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (cb_application_activate), NULL);

    lv_int_init();
    status = g_application_run (G_APPLICATION (app), 1, argv); // do not tell GTK about other arguments
    g_object_unref (app);

    return status;
}
