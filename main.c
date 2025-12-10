/*
 * Epic Rock Paper Scissors Battle - GTK4 (Fixed)
 *
 * Build (MSYS2 UCRT64):
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
    /* Game State */
    int current_round;
    int player_score;
    int computer_score;
    char player_name[50];

    /* UI Widgets */
    GtkWidget *stack;

    /* Screen 1: Name Input */
    GtkWidget *name_entry;
    GtkWidget *name_error_label;

    /* Screen 2: Game Battle */
    GtkWidget *round_label;
    GtkWidget *score_label;
    GtkWidget *feedback_label;
    GtkWidget *result_label;
    GtkWidget *choices_box;
    GtkWidget *next_round_btn;

    /* Screen 3: Final Result */
    GtkWidget *final_outcome_label;
    GtkWidget *final_score_label;
    GtkWidget *play_again_btn;
} AppData;

/* --- Forward declarations --- */
static void start_new_game(AppData *data);
static void start_next_round_ui(AppData *data);

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

/* Called after a short timeout to display final screen */
gboolean on_show_final_results(gpointer user_data) {
    AppData *data = (AppData *)user_data;
    char *outcome_text;
    char *score_text;

    /* FIXED: Correct GTK4 function names (no _name suffix) */
    gtk_widget_remove_css_class(data->final_outcome_label, "success");
    gtk_widget_remove_css_class(data->final_outcome_label, "error");
    gtk_widget_remove_css_class(data->final_outcome_label, "warning");

    if (data->player_score > data->computer_score) {
        outcome_text = g_strdup_printf("CHAMPION!\n%s defeated the machine!", data->player_name);
        gtk_widget_add_css_class(data->final_outcome_label, "success");
    } else if (data->computer_score > data->player_score) {
        outcome_text = g_strdup("DEFEAT!\nThe machines have won... for now.");
        gtk_widget_add_css_class(data->final_outcome_label, "error");
    } else {
        outcome_text = g_strdup("UNBELIEVABLE!\nA perfect tie!");
        gtk_widget_add_css_class(data->final_outcome_label, "warning");
    }

    score_text = g_strdup_printf("Final Score\n%s: %d  -  Computer: %d",
                                 data->player_name[0] ? data->player_name : "Player",
                                 data->player_score, data->computer_score);

    gtk_label_set_text(GTK_LABEL(data->final_outcome_label), outcome_text);
    gtk_label_set_text(GTK_LABEL(data->final_score_label), score_text);

    g_free(outcome_text);
    g_free(score_text);

    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "result_screen");

    return G_SOURCE_REMOVE; 
}

/* --- Game control --- */
void start_new_game(AppData *data) {
    data->current_round = 1;
    data->player_score = 0;
    data->computer_score = 0;

    gtk_label_set_text(GTK_LABEL(data->feedback_label), "Choose your weapon wisely...");
    gtk_label_set_text(GTK_LABEL(data->result_label), "");

    /* FIXED: Correct GTK4 function names */
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
    gtk_label_set_text(GTK_LABEL(data->feedback_label), "Choose your weapon wisely...");
    gtk_label_set_text(GTK_LABEL(data->result_label), "");

    gtk_widget_remove_css_class(data->result_label, "success");
    gtk_widget_remove_css_class(data->result_label, "error");
    gtk_widget_remove_css_class(data->result_label, "warning");

    update_round_display(data);

    gtk_widget_set_visible(data->choices_box, TRUE);
    gtk_widget_set_visible(data->next_round_btn, FALSE);
}

/* --- Game Logic --- */
void process_round(AppData *data, int user_choice) {
    int computer_choice = (rand() % 3) + 1;

    const char *user_str = (user_choice == 1) ? "ROCK" : (user_choice == 2) ? "PAPER" : "SCISSORS";
    const char *comp_str = (computer_choice == 1) ? "ROCK" : (computer_choice == 2) ? "PAPER" : "SCISSORS";

    int result = 0; /* 0=draw, 1=player, 2=computer */

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

    char *fb_text = g_strdup_printf("You threw %s!\nThe Computer countered with %s!", user_str, comp_str);
    gtk_label_set_text(GTK_LABEL(data->feedback_label), fb_text);
    g_free(fb_text);

    gtk_widget_remove_css_class(data->result_label, "success");
    gtk_widget_remove_css_class(data->result_label, "error");
    gtk_widget_remove_css_class(data->result_label, "warning");

    if (result == 0) {
        gtk_label_set_text(GTK_LABEL(data->result_label), "It's a deadlock! Great minds think alike.");
        gtk_widget_add_css_class(data->result_label, "warning");
    } else if (result == 1) {
        gtk_label_set_text(GTK_LABEL(data->result_label), "Victory! You crushed this round!");
        gtk_widget_add_css_class(data->result_label, "success");
    } else {
        gtk_label_set_text(GTK_LABEL(data->result_label), "Ouch! The computer got you this time.");
        gtk_widget_add_css_class(data->result_label, "error");
    }

    update_score_display(data);

    gtk_widget_set_visible(data->choices_box, FALSE);

    if (data->current_round < TOTAL_ROUNDS) {
        data->current_round++;
        gtk_button_set_label(GTK_BUTTON(data->next_round_btn), "Ready for the next round?");
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
    } else {
        g_strlcpy(data->player_name, name, sizeof(data->player_name));
        gtk_widget_set_visible(data->name_error_label, FALSE);
        g_free(name);
        start_new_game(data);
    }
}

/* FIXED: Callbacks just cast user_data to AppData* directly */
void on_rock_clicked(GtkButton *btn, gpointer user_data) { 
    process_round((AppData*)user_data, CHOICE_ROCK); 
}
void on_paper_clicked(GtkButton *btn, gpointer user_data) { 
    process_round((AppData*)user_data, CHOICE_PAPER); 
}
void on_scissors_clicked(GtkButton *btn, gpointer user_data) { 
    process_round((AppData*)user_data, CHOICE_SCISSORS); 
}
void on_next_round_clicked(GtkButton *btn, gpointer user_data) { 
    start_next_round_ui((AppData*)user_data); 
}
void on_play_again_clicked(GtkButton *btn, gpointer user_data) { 
    start_new_game((AppData*)user_data); 
}

/* --- CSS load (GTK4) --- */
void load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();

    const char *css =
        ".window { background-color: #d9d9d9; }"
        "#main_card { background-color: #ffffff; border-radius: 10px; padding: 24px; border: 1px solid #e5e7eb; }"
        "#title_label { font-size: 20pt; font-weight: bold; color: #1f2937; margin-bottom: 10px; }"
        "label { font-size: 11pt; color: #000000; margin-top: 5px; }"
        "entry { font-size: 11pt; padding: 8px; border: 1px solid #d1d5db; border-radius: 8px; }"
        "entry:focus { border: 2px solid #3b82f6; padding: 7px; }"
        "button { font-size: 11pt; font-weight: bold; border-radius: 8px; padding: 10px 16px; color: #ffffff; border: none; margin-top: 10px; }"
        "#start_btn, #next_btn, #again_btn { background-color: #105cd7; }"
        "#rock_btn, #paper_btn, #scissors_btn { background-color: #105cd7; margin-right: 5px; margin-left: 5px; }"
        "#start_btn:hover, #next_btn:hover, #again_btn:hover, #rock_btn:hover, #paper_btn:hover, #scissors_btn:hover { background-color: #3c77cf; }"
        "button:active { background-color: #00277c; }"
        "#result_label { font-size: 14pt; font-weight: bold; padding: 10px; }"
        "#final_outcome_label { font-size: 24pt; font-weight: bold; margin-bottom: 20px; }"
        "#final_score_label { font-size: 16pt; margin-bottom: 20px; }"
        ".success { color: #16a34a; }"
        ".error { color: #dc2626; }"
        ".warning { color: #ca8a04; }"
        "#name_error { color: #dc2626; font-size: 10pt; margin-top: 5px; }";

    /* FIXED: Removed the '-1' argument. This function now only takes 2 arguments. */
    gtk_css_provider_load_from_string(provider, css);

    if (display)
        gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider);
}

/* --- UI Construction --- */
GtkWidget* create_name_screen(AppData *data) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *lbl = gtk_label_new("Who dares to challenge the computer?");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), lbl);

    data->name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->name_entry), "Enter your warrior name...");
    gtk_box_append(GTK_BOX(box), data->name_entry);

    GtkWidget *btn = gtk_button_new_with_label("Let's Battle!");
    gtk_widget_set_name(btn, "start_btn");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_start_clicked), data);
    g_signal_connect(data->name_entry, "activate", G_CALLBACK(on_start_clicked), data);
    gtk_box_append(GTK_BOX(box), btn);

    data->name_error_label = gtk_label_new("");
    gtk_widget_set_name(data->name_error_label, "name_error");
    gtk_widget_set_visible(data->name_error_label, FALSE);
    gtk_box_append(GTK_BOX(box), data->name_error_label);

    return box;
}

GtkWidget* create_game_screen(AppData *data) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    data->round_label = gtk_label_new("Round 1: Fight!");
    gtk_widget_set_name(data->round_label, "title_label");
    gtk_box_append(GTK_BOX(vbox), data->round_label);

    data->score_label = gtk_label_new("Player: 0 | Computer: 0");
    gtk_box_append(GTK_BOX(vbox), data->score_label);

    data->feedback_label = gtk_label_new("Choose your weapon wisely...");
    gtk_box_append(GTK_BOX(vbox), data->feedback_label);

    data->result_label = gtk_label_new("");
    gtk_widget_set_name(data->result_label, "result_label");
    gtk_box_append(GTK_BOX(vbox), data->result_label);

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

    gtk_box_append(GTK_BOX(data->choices_box), btn_r);
    gtk_box_append(GTK_BOX(data->choices_box), btn_p);
    gtk_box_append(GTK_BOX(data->choices_box), btn_s);

    gtk_box_append(GTK_BOX(vbox), data->choices_box);

    data->next_round_btn = gtk_button_new_with_label("Next Round");
    gtk_widget_set_name(data->next_round_btn, "next_btn");
    gtk_widget_set_visible(data->next_round_btn, FALSE);
    g_signal_connect(data->next_round_btn, "clicked", G_CALLBACK(on_next_round_clicked), data);
    gtk_box_append(GTK_BOX(vbox), data->next_round_btn);

    return vbox;
}

GtkWidget* create_result_screen(AppData *data) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);

    data->final_outcome_label = gtk_label_new("");
    gtk_widget_set_name(data->final_outcome_label, "final_outcome_label");
    gtk_label_set_xalign(GTK_LABEL(data->final_outcome_label), 0.5);
    gtk_box_append(GTK_BOX(vbox), data->final_outcome_label);

    data->final_score_label = gtk_label_new("");
    gtk_widget_set_name(data->final_score_label, "final_score_label");
    gtk_label_set_xalign(GTK_LABEL(data->final_score_label), 0.5);
    gtk_box_append(GTK_BOX(vbox), data->final_score_label);

    data->play_again_btn = gtk_button_new_with_label("Rematch?");
    gtk_widget_set_name(data->play_again_btn, "again_btn");
    g_signal_connect(data->play_again_btn, "clicked", G_CALLBACK(on_play_again_clicked), data);
    gtk_box_append(GTK_BOX(vbox), data->play_again_btn);

    return vbox;
}

void activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;
    AppData *data = g_slice_new0(AppData);
    srand((unsigned int)time(NULL));

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Epic Rock Paper Scissors Battle");
    gtk_window_set_default_size(GTK_WINDOW(window), 450, 450);
    gtk_widget_set_name(window, "window");

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(card, "main_card");
    gtk_widget_set_halign(card, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(card, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(card, 400, 350);

    gtk_window_set_child(GTK_WINDOW(window), card);

    GtkWidget *title = gtk_label_new("Rock, Paper, Scissors Battle!");
    gtk_widget_set_name(title, "title_label");
    gtk_box_append(GTK_BOX(card), title);

    data->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_box_append(GTK_BOX(card), data->stack);

    gtk_stack_add_named(GTK_STACK(data->stack), create_name_screen(data), "name_screen");
    gtk_stack_add_named(GTK_STACK(data->stack), create_game_screen(data), "game_screen");
    gtk_stack_add_named(GTK_STACK(data->stack), create_result_screen(data), "result_screen");

    load_css();

    gtk_window_present(GTK_WINDOW(window));

    /* ensure initial visibility states */
    gtk_widget_set_visible(data->next_round_btn, FALSE);

    /* start on the name screen */
    gtk_stack_set_visible_child_name(GTK_STACK(data->stack), "name_screen");
}

/* --- main --- */
int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.rps", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}