#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h> // Include limits.h for INT_MAX

typedef struct {
    int pid;
    int at;  // Arrival Time
    int bt;  // Burst Time
    int priority;  // Priority (lower number indicates higher priority)
    int ct;  // Completion Time
    int tat; // Turnaround Time
    int wt;  // Waiting Time
    int is_completed; // Flag to check if process is completed
} Process;

Process processes[100];
int process_count = 0;

GtkWidget *pid_entry, *at_entry, *bt_entry, *priority_entry, *output_label, *gantt_canvas, *treeview;
GtkListStore *list_store;

void add_process(GtkWidget *widget, gpointer data) {
    int pid = atoi(gtk_entry_get_text(GTK_ENTRY(pid_entry)));
    int at = atoi(gtk_entry_get_text(GTK_ENTRY(at_entry)));
    int bt = atoi(gtk_entry_get_text(GTK_ENTRY(bt_entry)));
    int priority = atoi(gtk_entry_get_text(GTK_ENTRY(priority_entry)));

    if (pid <= 0 || at < 0 || bt <= 0 || priority < 0) {
        gtk_label_set_text(GTK_LABEL(output_label), "Please enter valid values.");
        return;
    }

    processes[process_count].pid = pid;
    processes[process_count].at = at;
    processes[process_count].bt = bt;
    processes[process_count].priority = priority;
    processes[process_count].is_completed = 0;
    process_count++;

    char message[256];
    snprintf(message, sizeof(message), "Process %d added. Click 'Simulate' to start.", pid);
    gtk_label_set_text(GTK_LABEL(output_label), message);

    gtk_entry_set_text(GTK_ENTRY(pid_entry), "");
    gtk_entry_set_text(GTK_ENTRY(at_entry), "");
    gtk_entry_set_text(GTK_ENTRY(bt_entry), "");
    gtk_entry_set_text(GTK_ENTRY(priority_entry), "");
}

void update_table() {
    gtk_list_store_clear(list_store);
    for (int i = 0; i < process_count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter,
                           0, processes[i].pid,
                           1, processes[i].at,
                           2, processes[i].bt,
                           3, processes[i].priority,
                           4, processes[i].ct,
                           5, processes[i].tat,
                           6, processes[i].wt,
                           -1);
    }
}

void simulate(GtkWidget *widget, gpointer data) {
    if (process_count == 0) {
        gtk_label_set_text(GTK_LABEL(output_label), "No processes to simulate!");
        return;
    }

    int st = 0; // Simulation time
    int total_tat = 0;
    int total_wt = 0;
    int completed = 0;

    // Clear Gantt chart
    gtk_widget_queue_draw(gantt_canvas);

    while (completed < process_count) {
        int idx = -1;
        int highest_priority = INT_MAX;

        // Find the process with the highest priority that has arrived and isn't completed
        for (int i = 0; i < process_count; i++) {
            if (processes[i].at <= st && !processes[i].is_completed && processes[i].priority < highest_priority) {
                highest_priority = processes[i].priority;
                idx = i;
            }
        }

        // If no process is ready, increment the simulation time
        if (idx == -1) {
            st++;
            continue;
        }

        // Process the selected job to completion
        st += processes[idx].bt;
        processes[idx].ct = st;
        processes[idx].tat = processes[idx].ct - processes[idx].at;
        processes[idx].wt = processes[idx].tat - processes[idx].bt;
        processes[idx].is_completed = 1;

        total_tat += processes[idx].tat;
        total_wt += processes[idx].wt;
        completed++;

        // Draw Gantt chart segment for each process
        gtk_widget_queue_draw(gantt_canvas);
        while (gtk_events_pending()) {
            gtk_main_iteration();
        }
        sleep(1); // Delay for visualization

        // Update output for each process completion
        char result_text[256];
        snprintf(result_text, sizeof(result_text), "Process %d: CT = %d, TAT = %d, WT = %d",
                 processes[idx].pid, processes[idx].ct, processes[idx].tat, processes[idx].wt);
        gtk_label_set_text(GTK_LABEL(output_label), result_text);
    }

    // Calculate average TAT and WT
    double avg_tat = (double)total_tat / process_count;
    double avg_wt = (double)total_wt / process_count;

    // Display final average results
    char final_result[512];
    snprintf(final_result, sizeof(final_result), "Average Turnaround Time: %.2f\nAverage Waiting Time: %.2f",
             avg_tat, avg_wt);
    gtk_label_set_text(GTK_LABEL(output_label), final_result);

    update_table(); // Update the table after simulation
}

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int scale_factor = 40;
    int rect_height = 50;

    for (int i = 0; i < process_count; i++) {
        if (processes[i].is_completed) {
            int x_start = (processes[i].ct - processes[i].bt) * scale_factor;
            int x_end = processes[i].ct * scale_factor;

            // Draw filled rectangle for the process
            cairo_set_source_rgb(cr, 0.678, 0.847, 0.902); // Light blue
            cairo_rectangle(cr, x_start, 10, x_end - x_start, rect_height);
            cairo_fill(cr);

            // Draw border around the rectangle
            cairo_set_source_rgb(cr, 0, 0, 0); // Black border color
            cairo_rectangle(cr, x_start, 10, x_end - x_start, rect_height);
            cairo_stroke(cr);

            // Draw process label
            cairo_set_source_rgb(cr, 0, 0, 0); // Black for text
            cairo_move_to(cr, (x_start + x_end) / 2 - 10, 10 + rect_height / 2);
            cairo_show_text(cr, g_strdup_printf("P%d", processes[i].pid));
        }
    }
    return FALSE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Priority Process Scheduler");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    pid_entry = gtk_entry_new();
    at_entry = gtk_entry_new();
    bt_entry = gtk_entry_new();
    priority_entry = gtk_entry_new();
    output_label = gtk_label_new("Add processes and click 'Simulate'.");
    gantt_canvas = gtk_drawing_area_new();
    gtk_widget_set_size_request(gantt_canvas, 1500, 300);

    list_store = gtk_list_store_new(7, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
    treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    for (int i = 0; i < 7; i++) {
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            (i == 0) ? "PID" : (i == 1) ? "AT" : (i == 2) ? "BT" : (i == 3) ? "Priority" : (i == 4) ? "CT" : (i == 5) ? "TAT" : "WT",
            renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    }

    GtkWidget *add_button = gtk_button_new_with_label("Add Process");
    GtkWidget *simulate_button = gtk_button_new_with_label("Simulate");

    g_signal_connect(add_button, "clicked", G_CALLBACK(add_process), NULL);
    g_signal_connect(simulate_button, "clicked", G_CALLBACK(simulate), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(gantt_canvas, "draw", G_CALLBACK(on_draw_event), NULL);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("PID:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), pid_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Arrival Time:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), at_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Burst Time:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), bt_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Priority:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), priority_entry, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), add_button, 0, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), simulate_button, 0, 5, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), output_label, 0, 6, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gantt_canvas, 0, 7, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), treeview, 2, 0, 1, 8);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
