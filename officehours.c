/*
    NAME : DARSHAK RAIRAKHIA
    ID   : 1001637657
*/


#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

/*** Constants that define parameters of the simulation ***/

#define MAX_SEATS 3        /* Number of seats in the professor's office */
#define professor_LIMIT 10 /* Number of students the professor can help before he needs a break */
#define MAX_STUDENTS 1000  /* Maximum number of students in the simulation */

#define CLASSA 0
#define CLASSB 1

/* TODO */
/* Add your synchronization variables here */

/* Basic information about simulation.  They are printed/checked at the end
 * and in assert statements during execution.
 *
 * You are responsible for maintaining the integrity of these variables in the
 * code that you develop.
 */

pthread_mutex_t lock_a, lock_b, lockal, lockbl, break1, lock1;
pthread_cond_t cond;

static int students_in_office;   /* Total numbers of students currently in the office */
static int classa_inoffice;      /* Total numbers of students from class A currently in the office */
static int classb_inoffice;      /* Total numbers of students from class B in the office */
static int students_since_break; // Total number of students in the class since the professor break
static int total_students;       /*total students entered the class till now*/
int student_type;
static int total_classA_students;
static int total_classB_students;


sem_t abc;

typedef struct
{
  int arrival_time;  // time between the arrival of this student and the previous student
  int question_time; // time the student needs to spend with the professor
  int student_id;
}
 student_info;

 static void take_break();
 void *professorthread(void *junk);
 void classa_enter();
 void classb_enter();
 static void ask_questions(int t);
 static void classa_leave();
 static void classb_leave();
 void* classa_student(void *si);
 void* classb_student(void *si);





/* Called at beginning of simulation.
 * TODO: Create/initialize all synchronization
 * variables and other global variables that you add.
 */
static int initialize(student_info *si, char *filename)
{
  students_in_office = 0;
  classa_inoffice = 0;
  classb_inoffice = 0;
  students_since_break = 0;
  total_students = 0;
  total_classA_students = 0;
  total_classB_students = 0;


  /* Initialize your synchronization variables (and
   * other variables you might use) here
   */


  /* Read in the data file and initialize the student array */
  FILE *fp;

  if((fp=fopen(filename, "r")) == NULL)
  {
    printf("Cannot open input file %s for reading.\n", filename);
    exit(1);
  }

  int i = 0;
  while ( (fscanf(fp, "%d%d\n", &(si[i].arrival_time), &(si[i].question_time))!=EOF) && i < MAX_STUDENTS )
  {
    i++;
  }

 fclose(fp);
 return i;
}

/* Code executed by professor to simulate taking a break
 * You do not need to add anything here.
 */
static void take_break()
{
    sleep(25);
  printf("The professor is taking a break now.\n");

  assert( students_in_office == 0 );
  students_since_break = 0;
  students_in_office += 1;
}

/* Code for the professor thread. This is fully implemented except for synchronization
 * with the students.  See the comments within the function for details.
 */

void *professorthread(void *junk)
{
  printf("The professor arrived and is starting his office hours\n");



  /* Loop while waiting for students to arrive. */
  while (1)
  {
        pthread_mutex_lock(&lock1);
      if(students_since_break == 10)
      {
          pthread_cond_wait(&cond,&lock1);

          // lock break mutex
          pthread_mutex_lock(&break1);

          take_break();
          // unlock brejak
          pthread_mutex_unlock(&break1);
        students_since_break = 0;

      }

      if(total_classA_students == 5 && total_classB_students == 0)
      {
          classb_enter();
      }

      if(total_classB_students == 5 && total_classA_students == 0)
      {
          classa_enter();
      }

      else if((students_in_office == 0 || classb_inoffice == 5 || classb_inoffice == 0 || students_in_office > 3) && (students_since_break < 10))
      {
          pthread_cond_signal(&cond);
      }

      else if((students_in_office == 0 || classa_inoffice == 5 || classa_inoffice == 0 || students_in_office > 3) && (students_since_break < 10))
      {
          pthread_cond_signal(&cond);
      }
      else if(students_since_break == 0)
      {
          pthread_cond_signal(&cond);
      }

      pthread_mutex_unlock(&lock1);






    /* TODO */
    /* Add code here to handle the student's request.             */
    /* Currently the body of the loop is empty. There's           */
    /* no communication between professor and students, i.e. all  */
    /* students are admitted without regard of the number         */
    /* of available seats, which class a student is in,           */
    /* and whether the professor needs a break. You need to add   */
    /* all of this.                                               */

  }

  pthread_exit(NULL);
}


/* Code executed by a class A student to enter the office.
 * You have to implement this.  Do not delete the assert() statements,
 * but feel free to add your own.
 */

void classa_enter()
{

  /* TODO */
  /* Request permission to enter the office.  You might also want to add  */
  /* synchronization for the simulations variables below                  */
  /*  YOUR CODE HERE.
  */
     sem_wait(&abc);
  pthread_mutex_lock(&lock1);

    printf("May I enter?\n");
    while(students_since_break == 10)
    {
        pthread_cond_wait(&cond, &lock1);
    }
    //Logic for taking only 3 class A
    //type students
    if (students_in_office < 3 )
    {
        if(classb_inoffice == 0)
       {


           printf("YES YOU MAY\n");

       }



    else

    {   while(classb_inoffice != 0 )
        {
    //        printf("NO YOU CAN'T\n");
            pthread_cond_wait(&cond,&lock1);

        }
    }
    }
//lock break mutex
pthread_mutex_lock(&break1);
            classa_inoffice += 1;
            students_in_office += 1;
            printf("Total students in office : %d\n",students_in_office);
            total_students += 1;
            printf("Total students entered till now : %d \n",total_students);
            students_since_break += 1;
            printf("Total students since break is %d\n",students_since_break);
            total_classA_students += 1;
pthread_mutex_unlock(&break1);


//  unlock break muex
    pthread_mutex_unlock(&lock1);
}

/* Code executed by a class B student to enter the office.
 * You have to implement this.  Do not delete the assert() statements,
 * but feel free to add your own.
 */
void classb_enter()
{
    pthread_mutex_lock(&lock1);
    printf("May I enter?\n");
    while(students_since_break == 10)
    {
        pthread_cond_wait(&cond, &lock1);
    }
    //Logic to take only 3 class b
    //students in the class
    if(students_in_office < 3 )
    {
        if(classa_inoffice == 0)
        {


            printf("YES YOU MAY\n");

        }






    else
     {

        while(classa_inoffice != 0 || students_in_office > 3)
        {
    //        printf("NO YOU CAN'T");
            pthread_cond_wait(&cond,&lock1);

        }
     }
    }

pthread_mutex_lock(&break1);
            classb_inoffice += 1;


            students_in_office += 1;
            printf("Total students : %d\n",students_in_office);
            total_students += 1;
            printf("Total students entered till now : %d\n",total_students);
            students_since_break += 1;
            printf("Total students since break : %d\n",students_since_break);
            total_classB_students += 1;

pthread_mutex_unlock(&break1);










   pthread_mutex_unlock(&lock1);


}

/* Code executed by a student to simulate the time he spends in the office asking questions
 * You do not need to add anything here.
 */
static void ask_questions(int t)
{
  sleep(t);
}


/* Code executed by a class A student when leaving the office.
 * You need to implement this.  Do not delete the assert() statements,
 * but feel free to add as many of your own as you like.
 */

static void classa_leave()
{
    pthread_mutex_lock(&lock1);

    sem_post(&abc);
  /*
   *  TODO
   *  YOUR CODE HERE.
   */

  students_in_office -= 1;
  classa_inoffice -= 1;

  pthread_mutex_unlock(&lock1);

}

/* Code executed by a class B student when leaving the office.
 * You need to implement this.  Do not delete the assert() statements,
 * but feel free to add as many of your own as you like.
 */
static void classb_leave()
{
    pthread_mutex_lock(&lock1);

    sem_post(&abc);

  /*
   * TODO
   * YOUR CODE HERE.
   */

  students_in_office -= 1;
  classb_inoffice -= 1;
  pthread_mutex_unlock(&lock1);


}

/* Main code for class A student threads.
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void* classa_student(void *si)
{
    pthread_mutex_lock(&lock1);
  student_info *s_info = (student_info*)si;

  /* enter office */
  classa_enter();

  printf("Student %d from class A enters the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classb_inoffice == 0 );

  /* ask questions  --- do not make changes to the 3 lines below*/
  printf("Student %d from class A starts asking questions for %d minutes\n", s_info->student_id, s_info->question_time);
  ask_questions(s_info->question_time);
  printf("Student %d from class A finishes asking questions and prepares to leave\n", s_info->student_id);

  /* leave office */
  classa_leave();

  printf("Student %d from class A leaves the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);
  pthread_mutex_unlock(&lock1);

  pthread_exit(NULL);
}

/* Main code for class B student threads.
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void* classb_student(void *si)
{
    pthread_mutex_lock(&lock1);
  student_info *s_info = (student_info*)si;

  /* enter office */
  classb_enter();

  printf("Student %d from class B enters the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);
  assert(classa_inoffice == 0 );

  printf("Student %d from class B starts asking questions for %d minutes\n", s_info->student_id, s_info->question_time);
  ask_questions(s_info->question_time);
  printf("Student %d from class B finishes asking questions and prepares to leave\n", s_info->student_id);

  /* leave office */
  classb_leave();

  printf("Student %d from class B leaves the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);

  pthread_mutex_unlock(&lock1);

  pthread_exit(NULL);
}

/* Main function sets up simulation and prints report
 * at the end.
 */
int main(int nargs, char **args)
{
  int i;
  int result;
  int student_type;
  int num_students;
  void *status;
  pthread_t professor_tid;
  pthread_t student_tid[MAX_STUDENTS];
  student_info s_info[MAX_STUDENTS];
  sem_init(&abc, 0 , 3);
//  pthread_mutex_init(&lock_a, NULL);
//  pthread_mutex_init(&lock_b, NULL);
//  pthread_mutex_init(&lockal, NULL);
//  pthread_mutex_init(&lockbl, NULL);
//  pthread_mutex_init(&lock1,NULL);

//  pthread_cond_init(&cond,NULL);

  if (nargs != 2)
  {
    printf("Usage: officehour <sample.txt>\n");
    return EINVAL;
  }

  num_students = initialize(s_info, args[1]);
  if (num_students > MAX_STUDENTS || num_students <= 0)
  {
    printf("Error:  Bad number of student threads. "
           "Maybe there was a problem with your input file?\n");
    return 1;
  }

  printf("Starting officehour simulation with %d students ...\n",
    num_students);

  result = pthread_create(&professor_tid, NULL, professorthread, NULL);

  if (result)
  {
    printf("officehour:  pthread_create failed for professor: %s\n", strerror(result));
    exit(1);
  }

  for (i=0; i < num_students; i++)
  {

    s_info[i].student_id = i;
    sleep(s_info[i].arrival_time);

    student_type = random() % 2;

    if (student_type == CLASSA)
    {
      result = pthread_create(&student_tid[i], NULL, classa_student, (void *)&s_info[i]);
    }
    else // student_type == CLASSB
    {
      result = pthread_create(&student_tid[i], NULL, classb_student, (void *)&s_info[i]);
    }

    if (result)
    {
      printf("officehour: thread_fork failed for student %d: %s\n",
            i, strerror(result));
      exit(1);
    }
  }

  /* wait for all student threads to finish */
  for (i = 0; i < num_students; i++)
  {
    pthread_join(student_tid[i], &status);
  }
    sem_post(&abc);
  /* tell the professor to finish. */
  pthread_cancel(professor_tid);

  printf("Office hour simulation done.\n");

  return 0;
}
