/* Program to evaluate candidate routines for Robotic Process Automation.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2020, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton
  file, the  program contained in this submission is completely my own
  individual work, except where explicitly noted by further comments that
  provide details otherwise.  I understand that work that has been developed
  by another student, or by me in collaboration with other students, or by
  non-students as a result of request, solicitation, or payment, may not be
  submitted for assessment in this subject.  I understand that submitting for
  assessment work developed by or in collaboration with other students or
  non-students constitutes Academic Misconduct, and may be penalized by mark
  deductions, or by other penalties determined via the University of
  Melbourne Academic Honesty Policy, as described at
  https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my
  work to other students, regardless of my intention or any undertakings made
  to me by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment
  specification to any form of code authoring or assignment tutoring service,
  or drawing the attention of others to such services and code that may have
  been made available via such a service, may be regarded as Student General
  Misconduct (interfering with the teaching activities of the University
  and/or inciting others to commit Academic Misconduct).  I understand that
  an allegation of Student General Misconduct may arise regardless of whether
  or not I personally make use of such solutions or sought benefit from such
  actions.

   Signed by: VINAY PINJANI | 1151832
   Dated:     30/10/2020

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

/* #define's -----------------------------------------------------------------*/

#define ASIZE 26
#define TRUE 1
#define FALSE 2
#define UNASSIGNED 0
/* type definitions ----------------------------------------------------------*/

// state (values of the 26 Boolean variables)
typedef unsigned char state_t[ASIZE];

// action
typedef struct action action_t;
struct action
{
    char name;      // action name
    state_t precon; // precondition
    state_t effect; // effect
};

// step in a trace
typedef struct step step_t;
struct step
{
    action_t *action; // pointer to an action performed at this step
    step_t *next;     // pointer to the next step in this trace
};

// trace (implemented as a linked list)
typedef struct
{
    step_t *head; // pointer to the step in the head of the trace
    step_t *tail; // pointer to the step in the tail of the trace
} trace_t;

/* function prototypes -------------------------------------------------------*/
trace_t *make_empty_trace(void);
trace_t *insert_at_tail(trace_t *, action_t *);
void free_trace(trace_t *);

/* my function prototypes ----------------------------------------------------*/

// add your function prototypes here ...
int analyze_input(trace_t *trace, action_t actions[]);
void initialize_state(state_t state);
void get_initial(state_t initial);
int count_valid(state_t initial, trace_t *trace);
void print_header(int n, int count, int valid);
void print_valid_states(trace_t *trace, state_t initial, int valid_count);
void print_state(state_t current, char name);
int check_precon(state_t precon, state_t current);
void update_current(state_t effect, state_t current);
int candidate_routine(action_t actions[], trace_t *trace, int advanced);
void update_precon(state_t trace_precon, state_t precon);
void print_sequence(struct step *start, int steps);
void check_modify(state_t trace_effect, state_t trace_precon, state_t routine_effect);
int mygetchar();

/* where it all happens ------------------------------------------------------*/
int main(int argc, char *argv[])
{
    char c;
    int valid, advanced;
    // array to store the list of actions
    action_t actions[ASIZE];
    // linked list stores the defined trace in order
    trace_t *trace = make_empty_trace();
    // function to perform stage 0 
    valid = analyze_input(trace, actions);

    // check for continuing to stage 1
    if ((c = mygetchar()) == '#' && valid)
    {
        printf("==STAGE 1===============================\n");
        advanced = 0;
        // gets the last character of input as return
        c = candidate_routine(actions, trace, advanced);
    }
    //check for continuing to stage 2
    if (c == '#' && valid)
    {
        // indicates advanced methods are to be used
        advanced = 1;
        printf("==STAGE 2===============================\n");
        // gets the last character of input as return
        c = candidate_routine(actions, trace, advanced);
        // checks if user input has ended
        if (c == EOF)
        {
            printf("==THE END===============================\n");
        }
    }
    free_trace(trace);
    return EXIT_SUCCESS; // we are done !!! algorithms are fun!!!
}

/* function definitions ------------------------------------------------------*/

// Adapted version of the make_empty_list function by Alistair Moffat:
// https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
// Data type and variable names changed
trace_t
    *
    make_empty_trace(void)
{
    trace_t *R;
    R = (trace_t *)malloc(sizeof(*R));
    assert(R != NULL);
    R->head = R->tail = NULL;
    return R;
}

// Adapted version of the insert_at_foot function by Alistair Moffat:
// https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
// Data type and variable names changed
trace_t
    *
    insert_at_tail(trace_t *R, action_t *addr)
{
    step_t *new;
    new = (step_t *)malloc(sizeof(*new));
    assert(R != NULL && new != NULL);
    new->action = addr;
    new->next = NULL;
    if (R->tail == NULL)
    { /* this is the first insertion into the trace */
        R->head = R->tail = new;
    }
    else
    {
        R->tail->next = new;
        R->tail = new;
    }
    return R;
}

// Adapted version of the free_list function by Alistair Moffat:
// https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
// Data type and variable names changed
void free_trace(trace_t *R)
{
    step_t *curr, *prev;
    assert(R != NULL);
    curr = R->head;
    while (curr)
    {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(R);
}

/* my function definitions ---------------------------------------------------*/

// add your function definitions here ...

/* performs stage 1 of the tasks where it reads the input commands and prints the analysis of the states and returns 1 if trace is valid*/
int analyze_input(trace_t *trace, action_t actions[])
{
    char c; //stores the read character
    int trace_max = 0, n_actions = 0, valid_count, is_valid = 0;
    state_t initial; //stores the initial state from the input
    get_initial(initial);

    c = mygetchar();

    // loop runs until all actions have been read
    while (c != '#')
    {
        action_t *next_action; // stores the value for the action
        next_action = (action_t *)malloc(sizeof(*next_action));

        // initializes both actions and effects to unassigned variables
        initialize_state(next_action->precon);
        initialize_state(next_action->effect);
        // read action lines and stores the relavant information in the array
        while ((c = mygetchar()) != ':' && c != '#')
        {
            next_action->precon[c - 'a'] = TRUE;
        }
        if (c == '#')
        {
            break;
        }
        while ((c = mygetchar()) != ':')
        {
            next_action->precon[c - 'a'] = FALSE;
        }
        while ((c = mygetchar()) != ':')
        {
            next_action->name = c;
        }
        while ((c = mygetchar()) != ':')
        {
            next_action->effect[c - 'a'] = TRUE;
        }
        while ((c = mygetchar()) != '\n')
        {
            next_action->effect[c - 'a'] = FALSE;
        }
        // stores the value at the ASCII position of the action name
        actions[next_action->name - 'A'] = *next_action;

        // appends the value of distinct actions
        n_actions++;

        // frees the memory allocated to the action
        free(next_action);
    }
    mygetchar(); // takes the newline value
    // loop reads the enterred trace and adds to the linked list
    while ((c = mygetchar()) != '\n')
    {
        // finds the corresponding actions and adds to linked list
        insert_at_tail(trace, &actions[c - 'A']);

        // appends the length of trace
        trace_max++;
    }

    valid_count = count_valid(initial, trace);
    // checks if all the actions in the trace are valid
    if (trace_max == valid_count)
    {
        // entire trace is declared valid
        is_valid = 1;
    }

    // prints the header with the analysis
    print_header(n_actions, trace_max, is_valid);
    // prints the initial state set by the user
    print_state(initial, '>');
    // prints all the states after apply effects
    print_valid_states(trace, initial, valid_count);

    return is_valid;
}

/*function initializes all variables in a state to unassigned*/
void initialize_state(state_t state)
{
    int i;
    for (i = 0; i < ASIZE; i++)
    {
        state[i] = UNASSIGNED;
    }
}

/*function to get the initial state set by the user*/
void get_initial(state_t initial)
{
    char c;
    int i;
    for (i = 0; i < ASIZE; i++)
    {
        initial[i] = FALSE; //initializes all values to false
    }
    while ((c = mygetchar()) != '#')
    {
        if (c != '\n')
        {
            //the position of the variable is set to true
            initial[c - 'a'] = TRUE;
        }
    }
}

/*function counts the length of valid states in the trace order*/
int count_valid(state_t initial, trace_t *trace)
{
    int valid = 0;                   // stores the number of valid moves
    state_t current;                 // stores the state after each action
    memcpy(current, initial, ASIZE); // copies the value of the initial state

    // traverses the linked list
    struct step *temp;
    temp = trace->head;
    //loop runs until the last action in the trace or when invalid point is found
    while (temp)
    {
        if (check_precon(temp->action->precon, current) == 1)
        {
            valid++;
            update_current(temp->action->effect, current);
        }
        else
        {
            break;
        }
        
        temp = temp->next;

        // points to the next action in trace
        // temp = temp->next;
    }
    // returns number of valid actions in order
    return valid;
}

/* function to print the analysis for stage 0 */
void print_header(int n, int count, int valid)
{
    printf("==STAGE 0===============================\n");
    printf("Number of distinct actions: %d\n", n);
    printf("Length of the input trace: %d\n", count);
    printf("Trace status: ");
    if (valid == 1)
    {
        printf("valid\n");
    }
    else
    {
        printf("invalid\n");
    }
    printf("----------------------------------------\n");
    printf("  abcdefghijklmnopqrstuvwxyz\n");
}

/* funtion to print all the states that are valid by checking their preconditions*/
void print_valid_states(trace_t *trace, state_t initial, int valid_count)
{
    int i = 0;
    state_t current;                 // stores the states after effects are applied
    memcpy(current, initial, ASIZE); // copies the initial state enterred by the user

    // traverses the linked list
    struct step *temp;
    temp = trace->head;
    // loop runs until the number of valid states is reached
    while (i < valid_count)
    {
        // updates the state using effects
        update_current(temp->action->effect, current);
        // prints the state and the action name
        print_state(current, temp->action->name);
        i++;
        // pointer points to the next state
        temp = temp->next;
    }
}

/*function to print a state and the action name*/
void print_state(state_t current, char name)
{
    printf("%c ", name); // prints the name of the action
    for (int i = 0; i < ASIZE; i++)
    {
        if (current[i] == FALSE)
        {
            printf("%d", 0); // prints '0' for all false states
        }
        else
        {
            printf("%d", current[i]);
        }
    }
    printf("\n");
}

/*function checks the precondion against the current state and returns a boolean*/
int check_precon(state_t precon, state_t current)
{
    int i;
    for (i = 0; i < ASIZE; i++)
    {
        // checks if variables differ in state and precondition
        if (precon[i] == TRUE && current[i] == FALSE)
        {
            return 0;
        }
        if (precon[i] == FALSE && current[i] == TRUE)
        {
            return 0;
        }
    }
    return 1;
}

// function to update the current state using the effects of the action
void update_current(state_t effect, state_t current)
{
    int i;
    for (i = 0; i < ASIZE; i++)
    {
        // sets the values of the current state according to the effects
        if (effect[i] == TRUE)
        {
            current[i] = TRUE;
        }
        if (effect[i] == FALSE)
        {
            current[i] = FALSE;
        }
    }
}

int mygetchar()
{
    int c;
    while ((c = getchar()) == '\r')
    {
    }
    return c;
}

/*function performs stage 1 and 2 of the tasks where it checks the entered subroutines in the sequence*/
int candidate_routine(action_t actions[], trace_t *trace, int advanced)
{
    char c;
    mygetchar();            //takes the value of newline
    int end_stage = 0;      // flag to denote if the stage ends
    state_t trace_precon;   // stores the cumalative precon of the subsequence
    state_t routine_effect; // stores the cumalative effect of the routine
    state_t trace_effect;   // stores the cumalative effect of the subsequence
    c = mygetchar();
    while (!end_stage)
    {
        printf("Candidate routine: ");
        initialize_state(routine_effect);
        //loop ends when the candidate routine is read
        while (c != '\n')
        {
            // updates effect of the candidate routine
            update_current(actions[c - 'A'].effect, routine_effect);
            printf("%c", c);
            c = mygetchar();
        }
        printf("\n");
        int pos = 0, steps;
        struct step *start, *sub;
        start = trace->head;
        // nested loop changes the position of the start of the subsequence
        while (start)
        {
            // only active when advanced candidate routines are searched
            if (advanced)
            {
                initialize_state(trace_precon);
            }
            // stores the length of the subsequence
            steps = 0;
            initialize_state(trace_effect);
            sub = start; // both pointers start from the head
            // loop ends when all the actions are read or when a subsequence is found
            while (sub)
            {
                // updates the effect of the subsequence
                update_current(sub->action->effect, trace_effect);
                // only active when advanced candidate routines are searched
                if (advanced)
                {
                    // updates the precondition of the subsequence
                    update_precon(trace_precon, sub->action->precon);
                    // checks if specific modifications are made in the trace
                    check_modify(trace_effect, trace_precon, routine_effect);
                }
                // compares the subsequence with the candidate routine
                if (memcmp(routine_effect, trace_effect, sizeof(routine_effect)) == 0)
                {
                    printf("%5d: ", pos);
                    print_sequence(start, steps);
                    printf("\n");
                    //next loop will start from the next position in trace
                    start = sub;
                    // increments the starting position by the length of the subsequence
                    pos += steps;
                    break;
                }
                //length of the subsequnce is incremented
                steps++;
                sub = sub->next;
            }
            // the starting position is incremented
            pos++;
            start = start->next;
        }
        // checks if all candidate routines have been read
        if ((c = mygetchar()) != '#' && c != EOF)
        {
            printf("----------------------------------------\n");
        }
        else
        {
            end_stage = 1;
        }
    }
    // returns the last read character
    return c;
}

/*function updates the precon if has not been updated before*/
void update_precon(state_t trace_precon, state_t precon)
{
    int i;
    for (i = 0; i < ASIZE; i++)
    {
        // checks if precon is previously unassigned
        if (trace_precon[i] == UNASSIGNED)
        {
            if (precon[i] != UNASSIGNED)
            {
                trace_precon[i] = precon[i];
            }
        }
    }
}

/*funtion to print the subsequences using the length of the subsequence*/
void print_sequence(struct step *start, int steps)
{
    // points to the starting position of the subsequence
    struct step *current = start;
    for (int i = 0; i <= steps; i++)
    {
        // prints the names of actions un the trace
        printf("%c", current->action->name);
        current = current->next;
    }
}

/*function checks if specific modifications are made using the cumalative preconditions*/
void check_modify(state_t trace_effect, state_t trace_precon, state_t routine_effect)
{
    int i;
    for (i = 0; i < ASIZE; i++)
    {
        //checks if the value has not been altered by the candidate routine
        if (routine_effect[i] == UNASSIGNED)
        {
            // the cumalative effect and precondition have the same value the effect is set to unassigned
            if (trace_effect[i] == TRUE && trace_precon[i] == TRUE)
            {
                trace_effect[i] = UNASSIGNED;
            }
            if (trace_effect[i] == FALSE && trace_precon[i] == FALSE)
            {
                trace_effect[i] = UNASSIGNED;
            }
        }
    }
}
/* ta-da-da-daa!!! -----------------------------------------------------------*/
/*algorithms are fun*/