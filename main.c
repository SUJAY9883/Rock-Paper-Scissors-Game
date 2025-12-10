/*
 * Epic Rock Paper Scissors Battle - Fixed
 * Build:
 * gcc main.c -o main.exe $(pkg-config --cflags --libs gtk4)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>

/* --- Game Constants --- */
#define CHOICE_ROCK 1
#define CHOICE_PAPER 2
#define CHOICE_SCISSORS 3
#define TOTAL_ROUNDS 3

/* --- Data Structure --- */
typedef struct {
    int current_round;
    int player_score;
    int computer_score;
    char player_name[50];

    GtkWidget *stack;

    /* Screen 1 (Login) */
    GtkWidget *name_entry;
    GtkWidget *name_error_label;

    /* Screen 2 (Game) */
    GtkWidget *round_label;
    GtkWidget *score_label;
    GtkWidget *feedback_label;
    GtkWidget *result_label;
    GtkWidget *choices_box;
    GtkWidget *next_round_btn;

    /* Screen 3 (Result) */
    GtkWidget *final_outcome_label;
    GtkWidget *final_score_label;
    GtkWidget *play_again_btn;
} AppData;

/* --- Forward Declarations --- */
static void start_new_game(AppData *data);
static void start_next_round_ui(AppData *data);
static void process_round(AppData *data, int user_choice);

/* --- Helpers --- */
void update_score_display(AppData *data) {
    char *text = g_strdup_printf("%s: %d  |  Computer: %d",
                                  data->player_name[0] ? data->player_name : "Player",
                                  data->player_score, data->computer_score);
    gtk_label_set_text(GTK_LABEL(data->score_label), text);
    g_free(text);
}

void update_round_display(AppData *data) {
    char *text;
    if (data->current_round <= TOTAL_ROUNDS) {
        text = g_strdup_printf("Round %d: Fight!", data->current_round);
    } else {
        text = g_strdup("Calculating Results...");
    }
    gtk_label_set_text(GTK_LABEL(data->round_label), text);
    g_free(text);
}

gboolean on_show_final_results(gpointer user_data) {
    AppData *data = (AppData *)user_data;
    char *outcome_text;
    char *score_text;

    gtk_widget_remove_css_class(data->final_outcome_label, "success");
    gtk_widget_remove_css_class(data->final_outcome_label, "error");
    gtk_widget_remove_css_class(data->final_outcome_label, "warning");

    if (data->player_score > data->computer_score) {
        outcome_text = g_strdup_printf("CHAMPION!\n%s wins!", data->player_name);
        gtk_widget_add_css_class(data->final_outcome_label, "success");
    } else if (data->computer_score > data->player_score) {
        outcome_text = g_strdup("DEFEAT!\nThe Computer won.");
        gtk_widget_add_css_class(data->final_outcome_label, "error");
    } else {
        outcome_text = g_strdup("DRAW GAME!");
        gtk_widget_add_css_class(data->final_outcome_label, "warning");
    }

    score_text = g_strdup_printf("Final Score: %d - %d", data->player_score, data->computer_score);

    gtk_label_set_text(GTK_LABEL(data->final_outcome_label), outcome_text);
    gtk_label_set_text(GTK_LABEL(data->final_score_label), score_text);

    g_free(outcome_text);
    g_free(score_text);

    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "result_screen");
    return G_SOURCE_REMOVE; 
}

/* --- Game Logic --- */
void start_new_game(AppData *data) {
    data->current_round = 1;
    data->player_score = 0;
    data->computer_score = 0;

    gtk_label_set_text(GTK_LABEL(data->feedback_label), "Make your move...");
    gtk_label_set_text(GTK_LABEL(data->result_label), "");
    
    gtk_widget_remove_css_class(data->result_label, "success");
    gtk_widget_remove_css_class(data->result_label, "error");
    gtk_widget_remove_css_class(data->result_label, "warning");

    update_round_display(data);
    update_score_display(data);

    gtk_widget_set_visible(data->choices_box, TRUE);
    gtk_widget_set_visible(data->next_round_btn, FALSE);

    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "game_screen");
}

void start_next_round_ui(AppData *data) {
    gtk_label_set_text(GTK_LABEL(data->feedback_label), "Make your move...");
    gtk_label_set_text(GTK_LABEL(data->result_label), "");
    
    gtk_widget_remove_css_class(data->result_label, "success");
    gtk_widget_remove_css_class(data->result_label, "error");
    gtk_widget_remove_css_class(data->result_label, "warning");

    update_round_display(data);
    update_score_display(data);

    gtk_widget_set_visible(data->choices_box, TRUE);
    gtk_widget_set_visible(data->next_round_btn, FALSE);
}

void process_round(AppData *data, int user_choice) {
    int computer_choice = (rand() % 3) + 1;
    const char *user_str = (user_choice == 1) ? "ROCK" : (user_choice == 2) ? "PAPER" : "SCISSORS";
    const char *comp_str = (computer_choice == 1) ? "ROCK" : (computer_choice == 2) ? "PAPER" : "SCISSORS";
    int result = 0; 

    if (user_choice == computer_choice) result = 0;
    else if ((user_choice == 1 && computer_choice == 3) ||
             (user_choice == 2 && computer_choice == 1) ||
             (user_choice == 3 && computer_choice == 2)) {
        result = 1;
        data->player_score++;
    } else {
        result = 2;
        data->computer_score++;
    }

    char *fb_text = g_strdup_printf("You: %s  vs  PC: %s", user_str, comp_str);
    gtk_label_set_text(GTK_LABEL(data->feedback_label), fb_text);
    g_free(fb_text);

    gtk_widget_remove_css_class(data->result_label, "success");
    gtk_widget_remove_css_class(data->result_label, "error");
    gtk_widget_remove_css_class(data->result_label, "warning");

    if (result == 0) {
        gtk_label_set_text(GTK_LABEL(data->result_label), "It's a Draw.");
        gtk_widget_add_css_class(data->result_label, "warning");
    } else if (result == 1) {
        gtk_label_set_text(GTK_LABEL(data->result_label), "You Won!");
        gtk_widget_add_css_class(data->result_label, "success");
    } else {
        gtk_label_set_text(GTK_LABEL(data->result_label), "Computer Won.");
        gtk_widget_add_css_class(data->result_label, "error");
    }

    update_score_display(data);
    gtk_widget_set_visible(data->choices_box, FALSE);

    if (data->current_round < TOTAL_ROUNDS) {
        data->current_round++;
        gtk_button_set_label(GTK_BUTTON(data->next_round_btn), "Next Round ->");
        gtk_widget_set_visible(data->next_round_btn, TRUE);
    } else {
        data->current_round++;
        g_timeout_add_seconds(1, on_show_final_results, data);
    }
}

/* --- Callbacks --- */
void on_start_clicked(GtkButton *btn, gpointer user_data) {
    AppData *data = (AppData *)user_data;
    char *name = NULL;
    g_object_get(G_OBJECT(data->name_entry), "text", &name, NULL);

    if (name == NULL || strlen(name) == 0) {
        gtk_label_set_text(GTK_LABEL(data->name_error_label), "Hold on! Every hero needs a name!");
        gtk_widget_set_visible(data->name_error_label, TRUE);
        if (name) g_free(name);
        return;
    }
    g_strlcpy(data->player_name, name, sizeof(data->player_name));
    gtk_widget_set_visible(data->name_error_label, FALSE);
    g_free(name);
    start_new_game(data);
}

void on_rock_clicked(GtkButton *btn, gpointer user_data) { process_round((AppData*)user_data, CHOICE_ROCK); }
void on_paper_clicked(GtkButton *btn, gpointer user_data) { process_round((AppData*)user_data, CHOICE_PAPER); }
void on_scissors_clicked(GtkButton *btn, gpointer user_data) { process_round((AppData*)user_data, CHOICE_SCISSORS); }
void on_next_round_clicked(GtkButton *btn, gpointer user_data) { start_next_round_ui((AppData*)user_data); }
void on_play_again_clicked(GtkButton *btn, gpointer user_data) { start_new_game((AppData*)user_data); }

/* --- CSS Styling --- */
void load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();

    const char *css =
        /* Main background - Grey */
        ".window-bg { background-color: #cfcfcf; }"
        
        /* The White Card */
        ".login-card { background-color: #ffffff; border-radius: 12px; padding: 30px; margin: 20px; box-shadow: 0px 4px 8px rgba(0,0,0,0.1); }"
        
        /* Typography */
        ".game-title { font-size: 16pt; font-weight: bold; color: #4a00e0; margin-bottom: 5px; }"
        ".welcome-text { font-size: 14pt; font-weight: bold; color: #2979ff; margin-bottom: 20px; }"
        ".input-label { font-size: 11pt; color: #555555; margin-bottom: 5px; }"
        ".round-header { font-size: 18pt; font-weight: bold; color: #6200ea; margin-bottom: 5px; }"
        ".score-info { font-size: 10pt; color: #666666; margin-bottom: 15px; }"

        /* Entry Field */
        ".styled-entry { background: #ffffff; border: 1px solid #aaa; border-radius: 4px; padding: 10px; color: #000; }"
        ".styled-entry:focus { border: 2px solid #2962ff; }"
        
        /* Start Button */
        "#start_btn { background-color: #1a237e; color: white; font-weight: bold; border-radius: 5px; padding: 10px; margin-top: 15px; }"
        "#start_btn:hover { background-color: #283593; }"

        /* Choice Buttons (Rock/Paper/Scissors) */
        ".choice-btn { background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px; padding: 10px; box-shadow: 0 2px 2px rgba(0,0,0,0.05); }"
        ".choice-btn:hover { background-color: #e9ecef; border-color: #adb5bd; }"
        ".choice-emoji { font-size: 36px; }"
        ".choice-label { font-weight: bold; color: #333; font-size: 16px; margin-top: 5px; }"
        
        /* Footer Text */
        ".footer-tip { font-size: 9pt; color: #888888; margin-top: 15px; }"
        ".footer-credit { font-size: 8pt; color: #555555; margin-top: 5px; font-weight: bold; }"
        
        /* Game Screen Elements */
        ".success { color: #00c853; font-weight: bold; font-size: 14pt; }"
        ".error { color: #d50000; font-weight: bold; font-size: 11pt; }"
        ".warning { color: #ffab00; font-weight: bold; font-size: 14pt; }";

    gtk_css_provider_load_from_string(provider, css);

    if (display)
        gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

/* --- UI Construction --- */

/* * THIS IS THE MISSING FUNCTION THAT CAUSED THE ERROR 
 */
GtkWidget* create_choice_button(const char *emoji, const char *label_text, GCallback callback, AppData *data) {
    GtkWidget *btn = gtk_button_new();
    gtk_widget_add_css_class(btn, "choice-btn");
    
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);

    GtkWidget *lbl_emoji = gtk_label_new(emoji);
    gtk_widget_add_css_class(lbl_emoji, "choice-emoji");
    
    GtkWidget *lbl_text = gtk_label_new(label_text);
    gtk_widget_add_css_class(lbl_text, "choice-label");

    gtk_box_append(GTK_BOX(box), lbl_emoji);
    gtk_box_append(GTK_BOX(box), lbl_text);

    gtk_button_set_child(GTK_BUTTON(btn), box);
    g_signal_connect(btn, "clicked", callback, data);

    return btn;
}

/* 1. Login/Name Screen */
GtkWidget* create_name_screen(AppData *data) {
    GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(center_box, TRUE);
    gtk_widget_set_hexpand(center_box, TRUE);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_add_css_class(card, "login-card");
    gtk_widget_set_size_request(card, 350, -1);
    gtk_box_append(GTK_BOX(center_box), card);

    GtkWidget *title_lbl = gtk_label_new("🎮 ROCK PAPER SCISSORS");
    gtk_widget_add_css_class(title_lbl, "game-title");
    gtk_box_append(GTK_BOX(card), title_lbl);

    GtkWidget *welcome_lbl = gtk_label_new("Welcome!");
    gtk_widget_add_css_class(welcome_lbl, "welcome-text");
    gtk_box_append(GTK_BOX(card), welcome_lbl);

    GtkWidget *q_lbl = gtk_label_new("Who dares to challenge the computer?");
    gtk_widget_add_css_class(q_lbl, "input-label");
    gtk_widget_set_halign(q_lbl, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(card), q_lbl);

    data->name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->name_entry), "Enter your warrior name...");
    gtk_widget_add_css_class(data->name_entry, "styled-entry");
    gtk_box_append(GTK_BOX(card), data->name_entry);

    GtkWidget *btn = gtk_button_new_with_label("Let's Battle!");
    gtk_widget_set_name(btn, "start_btn");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_start_clicked), data);
    g_signal_connect(data->name_entry, "activate", G_CALLBACK(on_start_clicked), data);
    gtk_box_append(GTK_BOX(card), btn);

    data->name_error_label = gtk_label_new("");
    gtk_widget_add_css_class(data->name_error_label, "error");
    gtk_box_append(GTK_BOX(card), data->name_error_label);

    GtkWidget *tip_lbl = gtk_label_new("💡 Tip: Hey hero... don't forget to tell me who you are!");
    gtk_widget_add_css_class(tip_lbl, "footer-tip");
    gtk_box_append(GTK_BOX(card), tip_lbl);

    GtkWidget *credit_lbl = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(credit_lbl, "footer-credit");
    gtk_box_append(GTK_BOX(card), credit_lbl);

    return center_box;
}

/* 2. Game Screen */
GtkWidget* create_game_screen(AppData *data) {
    GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(center_box, TRUE);
    gtk_widget_set_hexpand(center_box, TRUE);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_add_css_class(card, "login-card"); 
    gtk_widget_set_size_request(card, 380, -1);
    gtk_box_append(GTK_BOX(center_box), card);

    GtkWidget *title_lbl = gtk_label_new("🎮 ROCK PAPER SCISSORS");
    gtk_widget_add_css_class(title_lbl, "game-title");
    gtk_box_append(GTK_BOX(card), title_lbl);

    data->round_label = gtk_label_new("Round 1: Fight!");
    gtk_widget_add_css_class(data->round_label, "round-header");
    gtk_box_append(GTK_BOX(card), data->round_label);

    data->score_label = gtk_label_new("Player: 0 | Computer: 0");
    gtk_widget_add_css_class(data->score_label, "score-info");
    gtk_box_append(GTK_BOX(card), data->score_label);

    data->feedback_label = gtk_label_new("Make your move...");
    gtk_widget_set_margin_bottom(data->feedback_label, 15);
    gtk_box_append(GTK_BOX(card), data->feedback_label);

    data->choices_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_widget_set_halign(data->choices_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(data->choices_box, 10);

    /* Custom Buttons - USING EMOJIS */
    GtkWidget *btn_rock = create_choice_button("✊", "Rock", G_CALLBACK(on_rock_clicked), data);
    GtkWidget *btn_paper = create_choice_button("✋", "Paper", G_CALLBACK(on_paper_clicked), data);
    GtkWidget *btn_scissors = create_choice_button("✌️", "Scissors", G_CALLBACK(on_scissors_clicked), data);
    
    gtk_widget_set_size_request(btn_rock, 80, 80);
    gtk_widget_set_size_request(btn_paper, 80, 80);
    gtk_widget_set_size_request(btn_scissors, 80, 80);

    gtk_box_append(GTK_BOX(data->choices_box), btn_rock);
    gtk_box_append(GTK_BOX(data->choices_box), btn_paper);
    gtk_box_append(GTK_BOX(data->choices_box), btn_scissors);

    gtk_box_append(GTK_BOX(card), data->choices_box);

    data->result_label = gtk_label_new("");
    gtk_box_append(GTK_BOX(card), data->result_label);

    data->next_round_btn = gtk_button_new_with_label("Next");
    gtk_widget_set_name(data->next_round_btn, "start_btn");
    g_signal_connect(data->next_round_btn, "clicked", G_CALLBACK(on_next_round_clicked), data);
    gtk_box_append(GTK_BOX(card), data->next_round_btn);

    GtkWidget *credit_lbl = gtk_label_new("Developed by SUJAY PAUL");
    gtk_widget_add_css_class(credit_lbl, "footer-credit");
    gtk_box_append(GTK_BOX(card), credit_lbl);

    return center_box;
}

/* 3. Result Screen */
GtkWidget* create_result_screen(AppData *data) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(vbox, TRUE);
    gtk_widget_set_hexpand(vbox, TRUE);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_add_css_class(card, "login-card");
    gtk_widget_set_size_request(card, 350, -1);
    gtk_box_append(GTK_BOX(vbox), card);

    data->final_outcome_label = gtk_label_new("");
    gtk_widget_set_halign(data->final_outcome_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(card), data->final_outcome_label);

    data->final_score_label = gtk_label_new("");
    gtk_widget_set_halign(data->final_score_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(card), data->final_score_label);

    data->play_again_btn = gtk_button_new_with_label("Play Again");
    gtk_widget_set_name(data->play_again_btn, "start_btn");
    g_signal_connect(data->play_again_btn, "clicked", G_CALLBACK(on_play_again_clicked), data);
    gtk_box_append(GTK_BOX(card), data->play_again_btn);

    return vbox;
}

void activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;
    AppData *data = g_slice_new0(AppData);
    srand((unsigned int)time(NULL));

    GtkWidget *window = gtk_application_window_new(app);
    gtk_widget_set_size_request(window, 800, 600);
    
    GtkWidget *header = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(window), header);
    gtk_window_set_title(GTK_WINDOW(window), "Epic Rock Paper Scissors Battle");

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(main_box, "window-bg");
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    data->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    gtk_widget_set_vexpand(data->stack, TRUE);
    gtk_widget_set_hexpand(data->stack, TRUE);

    gtk_box_append(GTK_BOX(main_box), data->stack);

    gtk_stack_add_named(GTK_STACK(data->stack), create_name_screen(data), "name_screen");
    gtk_stack_add_named(GTK_STACK(data->stack), create_game_screen(data), "game_screen");
    gtk_stack_add_named(GTK_STACK(data->stack), create_result_screen(data), "result_screen");

    load_css();

    gtk_window_present(GTK_WINDOW(window));

    gtk_widget_set_visible(data->next_round_btn, FALSE);
    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "name_screen");
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.rps", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}