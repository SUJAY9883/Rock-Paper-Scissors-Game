/*
 * GTK3 Rock-Paper-Scissors Game
 *
 * This C program converts the console logic into a GUI.
 *
 * --- How to Compile ---
 * gcc -o gtk_rps gtk_rps.c $(pkg-config --cflags --libs gtk+-3.0)
 *
 * --- Run ---
 * ./gtk_rps
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>

// --- Game Constants ---
#define CHOICE_ROCK 1
#define CHOICE_PAPER 2
#define CHOICE_SCISSORS 3
#define TOTAL_ROUNDS 3

// --- Data Structure ---
typedef struct {
    // Game State
    int current_round;
    int player_score;
    int computer_score;
    gchar player_name[50];

    // UI Widgets
    GtkWidget *stack;
    
    // Name Screen Widgets
    GtkWidget *name_entry;
    GtkWidget *name_error_label;

    // Game Screen Widgets
    GtkWidget *round_label;       // "ROUND NUMBER 1"
    GtkWidget *score_label;       // "Name: 0 | Computer: 0"
    GtkWidget *feedback_label;    // "Computer chose..."
    GtkWidget *result_label;      // "YOU WIN! / DRAW"
    
    GtkWidget *choices_box;       // Container for R/P/S buttons
    GtkWidget *next_round_btn;    // Button to go to next round
    GtkWidget *play_again_btn;    // Button to restart whole game
    
} AppData;

// --- Helper Functions ---

void update_score_display(AppData *data) {
    gchar *text = g_strdup_printf("%s: %d  |  Computer: %d", 
                                  data->player_name, data->player_score, data->computer_score);
    gtk_label_set_text(GTK_LABEL(data->score_label), text);
    g_free(text);
}

void update_round_display(AppData *data) {
    gchar *text;
    if (data->current_round <= TOTAL_ROUNDS) {
        text = g_strdup_printf("ROUND %d OF %d", data->current_round, TOTAL_ROUNDS);
    } else {
        text = g_strdup("GAME OVER");
    }
    gtk_label_set_text(GTK_LABEL(data->round_label), text);
    g_free(text);
}

void start_new_game(AppData *data) {
    data->current_round = 1;
    data->player_score = 0;
    data->computer_score = 0;

    // Reset UI
    gtk_label_set_text(GTK_LABEL(data->feedback_label), "Make your move!");
    gtk_label_set_text(GTK_LABEL(data->result_label), "");
    
    // Reset Styling
    GtkStyleContext *context = gtk_widget_get_style_context(data->result_label);
    gtk_style_context_remove_class(context, "success");
    gtk_style_context_remove_class(context, "error");
    gtk_style_context_remove_class(context, "warning");

    update_round_display(data);
    update_score_display(data);

    // Show choices, hide next/play again
    gtk_widget_show(data->choices_box);
    gtk_widget_hide(data->next_round_btn);
    gtk_widget_hide(data->play_again_btn);
}

void start_next_round_ui(AppData *data) {
    // Clear previous round results
    gtk_label_set_text(GTK_LABEL(data->feedback_label), "Make your move!");
    gtk_label_set_text(GTK_LABEL(data->result_label), "");
    
    GtkStyleContext *context = gtk_widget_get_style_context(data->result_label);
    gtk_style_context_remove_class(context, "success");
    gtk_style_context_remove_class(context, "error");
    gtk_style_context_remove_class(context, "warning");

    update_round_display(data);
    
    // Show buttons again
    gtk_widget_show(data->choices_box);
    gtk_widget_hide(data->next_round_btn);
}

// --- Game Logic ---

void process_round(AppData *data, int user_choice) {
    // 1. Generate Computer Choice
    int computer_choice = (rand() % 3) + 1;
    
    // 2. Strings for display
    const char *user_str = (user_choice == 1) ? "ROCK" : (user_choice == 2) ? "PAPER" : "SCISSORS";
    const char *comp_str = (computer_choice == 1) ? "ROCK" : (computer_choice == 2) ? "PAPER" : "SCISSORS";

    // 3. Determine Winner
    int result = 0; // 0=Draw, 1=Player Win, 2=Computer Win
    
    if (user_choice == computer_choice) {
        result = 0;
    } else if ((user_choice == 1 && computer_choice == 3) || 
               (user_choice == 2 && computer_choice == 1) || 
               (user_choice == 3 && computer_choice == 2)) {
        result = 1;
        data->player_score++;
    } else {
        result = 2;
        data->computer_score++;
    }

    // 4. Update UI Feedback
    gchar *fb_text = g_strdup_printf("You chose %s.\nComputer chose %s.", user_str, comp_str);
    gtk_label_set_text(GTK_LABEL(data->feedback_label), fb_text);
    g_free(fb_text);

    GtkStyleContext *ctx = gtk_widget_get_style_context(data->result_label);
    
    if (result == 0) {
        gtk_label_set_text(GTK_LABEL(data->result_label), "IT'S A DRAW!");
        gtk_style_context_add_class(ctx, "warning");
    } else if (result == 1) {
        gtk_label_set_text(GTK_LABEL(data->result_label), "YOU WIN THIS ROUND!");
        gtk_style_context_add_class(ctx, "success");
    } else {
        gtk_label_set_text(GTK_LABEL(data->result_label), "YOU LOSE THIS ROUND!");
        gtk_style_context_add_class(ctx, "error");
    }

    update_score_display(data);

    // 5. Handle Round Progression
    gtk_widget_hide(data->choices_box); // Hide input buttons
    
    if (data->current_round < TOTAL_ROUNDS) {
        // Prep for next round
        data->current_round++;
        gtk_button_set_label(GTK_BUTTON(data->next_round_btn), "Next Round");
        gtk_widget_show(data->next_round_btn);
    } else {
        // Game Over Logic
        data->current_round++; // To show "Game Over"
        update_round_display(data);
        
        gchar *final_msg;
        if (data->player_score > data->computer_score) {
            final_msg = g_strdup_printf("CONGRATULATIONS!\nYOU WIN THE GAME!\nFinal Score: %d - %d", 
                                        data->player_score, data->computer_score);
            gtk_style_context_add_class(ctx, "success");
        } else if (data->computer_score > data->player_score) {
            final_msg = g_strdup_printf("GAME OVER!\nCOMPUTER WINS!\nFinal Score: %d - %d", 
                                        data->player_score, data->computer_score);
            gtk_style_context_add_class(ctx, "error");
        } else {
            final_msg = g_strdup_printf("GAME OVER!\nIT'S A DRAW!\nFinal Score: %d - %d", 
                                        data->player_score, data->computer_score);
            gtk_style_context_add_class(ctx, "warning");
        }
        
        gtk_label_set_text(GTK_LABEL(data->result_label), final_msg);
        g_free(final_msg);
        
        gtk_widget_show(data->play_again_btn);
    }
}

// --- Callbacks ---

void on_start_clicked(GtkButton *btn, AppData *data) {
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(data->name_entry));
    if (strlen(name) == 0) {
        gtk_label_set_text(GTK_LABEL(data->name_error_label), "NAME CANNOT BE EMPTY.");
        gtk_widget_show(data->name_error_label);
    } else {
        g_strlcpy(data->player_name, name, sizeof(data->player_name));
        start_new_game(data);
        gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "game_screen");
    }
}

void on_rock_clicked(GtkButton *btn, AppData *data) { process_round(data, CHOICE_ROCK); }
void on_paper_clicked(GtkButton *btn, AppData *data) { process_round(data, CHOICE_PAPER); }
void on_scissors_clicked(GtkButton *btn, AppData *data) { process_round(data, CHOICE_SCISSORS); }

void on_next_round_clicked(GtkButton *btn, AppData *data) {
    start_next_round_ui(data);
}

void on_play_again_clicked(GtkButton *btn, AppData *data) {
    start_new_game(data);
}

// --- UI Construction ---

void load_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    // Reusing the exact same CSS style
    const char *css =
        ".window { background-color: #d9d9d9; }"
        "#main_card { background-color: #ffffff; border-radius: 10px; padding: 24px; border: 1px solid #e5e7eb; }"
        "#title_label { font-size: 20pt; font-weight: bold; color: #1f2937; margin-bottom: 10px; }"
        "label { font-size: 11pt; color: #000000; margin-top: 5px; }"
        "entry { font-size: 11pt; padding: 8px; border: 1px solid #d1d5db; border-radius: 8px; }"
        "entry:focus { border: 2px solid #3b82f6; padding: 7px; }"
        
        /* Buttons */
        "button { font-size: 11pt; font-weight: bold; border-radius: 8px; padding: 10px 16px; color: white; border: none; margin-top: 10px; transition: background-color 0.2s; }"
        "#start_btn, #next_btn, #again_btn { background-color: #3b82f6; }"
        "#rock_btn, #paper_btn, #scissors_btn { background-color: #3b82f6; margin-right: 5px; margin-left: 5px; }"
        
        "button:hover { background-color: #60a5fa; }"
        "button:active { background-color: #2563eb; }"
        
        /* Feedback Colors */
        "#result_label { font-size: 14pt; font-weight: bold; padding: 10px; }"
        ".success { color: #16a34a; }"
        ".error { color: #dc2626; }"
        ".warning { color: #ca8a04; }"
        "#name_error { color: #dc2626; font-size: 10pt; margin-top: 5px; }";
        
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    g_object_unref(provider);
}

GtkWidget* create_name_screen(AppData *data) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget *lbl = gtk_label_new("Enter Your Name:");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box), lbl, FALSE, FALSE, 0);

    data->name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->name_entry), "Player Name...");
    gtk_box_pack_start(GTK_BOX(box), data->name_entry, FALSE, FALSE, 0);

    GtkWidget *btn = gtk_button_new_with_label("Start Game");
    gtk_widget_set_name(btn, "start_btn");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_start_clicked), data);
    g_signal_connect(data->name_entry, "activate", G_CALLBACK(on_start_clicked), data);
    gtk_box_pack_start(GTK_BOX(box), btn, FALSE, FALSE, 0);

    data->name_error_label = gtk_label_new("");
    gtk_widget_set_name(data->name_error_label, "name_error");
    gtk_widget_set_no_show_all(data->name_error_label, TRUE);
    gtk_box_pack_start(GTK_BOX(box), data->name_error_label, FALSE, FALSE, 0);

    return box;
}

GtkWidget* create_game_screen(AppData *data) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    // Round Header
    data->round_label = gtk_label_new("ROUND NUMBER 1");
    gtk_widget_set_name(data->round_label, "title_label");
    gtk_box_pack_start(GTK_BOX(vbox), data->round_label, FALSE, FALSE, 0);

    // Score
    data->score_label = gtk_label_new("Player: 0 | Computer: 0");
    gtk_box_pack_start(GTK_BOX(vbox), data->score_label, FALSE, FALSE, 0);

    // Feedback Area
    data->feedback_label = gtk_label_new("Make your move!");
    gtk_box_pack_start(GTK_BOX(vbox), data->feedback_label, FALSE, FALSE, 10);
    
    data->result_label = gtk_label_new("");
    gtk_widget_set_name(data->result_label, "result_label");
    gtk_box_pack_start(GTK_BOX(vbox), data->result_label, FALSE, FALSE, 5);

    // --- Choices Area ---
    data->choices_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(data->choices_box, GTK_ALIGN_CENTER);
    
    GtkWidget *btn_r = gtk_button_new_with_label("ROCK");
    GtkWidget *btn_p = gtk_button_new_with_label("PAPER");
    GtkWidget *btn_s = gtk_button_new_with_label("SCISSORS");
    
    gtk_widget_set_name(btn_r, "rock_btn");
    gtk_widget_set_name(btn_p, "paper_btn");
    gtk_widget_set_name(btn_s, "scissors_btn");

    g_signal_connect(btn_r, "clicked", G_CALLBACK(on_rock_clicked), data);
    g_signal_connect(btn_p, "clicked", G_CALLBACK(on_paper_clicked), data);
    g_signal_connect(btn_s, "clicked", G_CALLBACK(on_scissors_clicked), data);

    gtk_box_pack_start(GTK_BOX(data->choices_box), btn_r, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(data->choices_box), btn_p, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(data->choices_box), btn_s, TRUE, TRUE, 0);
    
    gtk_box_pack_start(GTK_BOX(vbox), data->choices_box, FALSE, FALSE, 10);

    // --- Next / Play Again Buttons ---
    data->next_round_btn = gtk_button_new_with_label("Next Round");
    gtk_widget_set_name(data->next_round_btn, "next_btn");
    gtk_widget_set_no_show_all(data->next_round_btn, TRUE);
    g_signal_connect(data->next_round_btn, "clicked", G_CALLBACK(on_next_round_clicked), data);
    gtk_box_pack_start(GTK_BOX(vbox), data->next_round_btn, FALSE, FALSE, 0);

    data->play_again_btn = gtk_button_new_with_label("Play Again?");
    gtk_widget_set_name(data->play_again_btn, "again_btn");
    gtk_widget_set_no_show_all(data->play_again_btn, TRUE);
    g_signal_connect(data->play_again_btn, "clicked", G_CALLBACK(on_play_again_clicked), data);
    gtk_box_pack_start(GTK_BOX(vbox), data->play_again_btn, FALSE, FALSE, 0);

    return vbox;
}

void activate(GtkApplication *app, gpointer user_data) {
    AppData *data = g_slice_new0(AppData);
    srand(time(0));

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Rock Paper Scissors");
    gtk_window_set_default_size(GTK_WINDOW(window), 450, 400);
    gtk_widget_set_name(window, "window");

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(card, "main_card");
    gtk_widget_set_halign(card, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(card, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(card, 400, -1);
    gtk_container_add(GTK_CONTAINER(window), card);

    // Title
    GtkWidget *title = gtk_label_new("Rock-Paper-Scissors");
    gtk_widget_set_name(title, "title_label");
    gtk_box_pack_start(GTK_BOX(card), title, FALSE, FALSE, 10);

    // Stack
    data->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_box_pack_start(GTK_BOX(card), data->stack, TRUE, TRUE, 0);

    gtk_stack_add_named(GTK_STACK(data->stack), create_name_screen(data), "name_screen");
    gtk_stack_add_named(GTK_STACK(data->stack), create_game_screen(data), "game_screen");

    load_css();
    gtk_widget_show_all(window);
    
    // Initial hides
    gtk_widget_hide(data->next_round_btn);
    gtk_widget_hide(data->play_again_btn);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.rps", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}