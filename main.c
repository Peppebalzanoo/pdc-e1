#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "mpi.h"


int* read_input_numbers(int* punt_vec, int* num, char** punt_argv);

int* generate_random_numbers(int* punt_vec, int* num);

bool is_power_of_two(int* num_proc);

int calculate_partial_sum(int* punt_vec, int* num);

void print_vector(int* vec, int* num);

void sum_vector(int* vec, int* num);

void strategy1(int* curr_id_proc, int* num_proc, int partial_sum, MPI_Status* mpi_status);

void strategy2(int* curr_id_proc, int* num_proc, int partial_sum, MPI_Status* mpi_status);

void strategy3(int* curr_id_proc, int* num_proc, int partial_sum, MPI_Status* mpi_status);

/* ****************************************************************************************************************** */

int main(int argc, char** argv) {
    int curr_id_proc, num_proc, num_elem, num_loc, num_rest, count_elem, index;
    int *vec = NULL, *vec_loc = NULL;
    int communication_tag = 0;
    MPI_Status mpi_status;

    int strategia = atoi(*(argv+1));

    MPI_Init(&argc, &argv);

    /*******  Start MPI Program *******/
    MPI_Comm_rank(MPI_COMM_WORLD, &curr_id_proc);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);


    // Process P0 read (num_elem, vec: vector_of_elements) and send information at all process
    if(curr_id_proc == 0){

        num_elem = atoi(*(argv + 2)); //In 2 third position of argv[] we find the processes number

        //Allocate memory for the input elements
        vec = (int*)malloc((num_elem) * sizeof(int));

        if (num_elem <= 20){
            vec = read_input_numbers(vec, &num_elem, argv);
        }
        else{
            vec = generate_random_numbers(vec, &num_elem);
        }
        print_vector(vec, &num_elem);
        sum_vector(vec, &num_elem);
    }
    MPI_Bcast(&num_elem, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //All processes calculate num_loc and num_rest
    num_loc = (num_elem / num_proc);  // Number of elements for current process
    num_rest = (num_elem % num_proc); // Number of elements to assign at {P0,...,(P_num_rest - 1)} processes

    if (curr_id_proc < num_rest){ // Check if the current process is in {P0,...,(P_num_rest - 1)}
        num_loc += 1;
    }

    //P0 send elements for each {P1,...,P_num_proc} processes
    if(curr_id_proc == 0){

        vec_loc = vec; //P0 can use directly vec for vec_loc
        count_elem = num_loc;
        index = 0;

        for(int id = 1; id < num_proc; id++){
            index += count_elem;
            communication_tag = 10 * id;

            //Check if  we have reached the P_num_rest process
            //Processes {P_num_rest,...,P_num_proc} receive count - 1 element
            if(id == num_rest){
                count_elem -= 1;
            }
            MPI_Send((vec_loc + index), count_elem, MPI_INT, id, communication_tag, MPI_COMM_WORLD);
        }
    }
    else{
        //For each process != P0, {P1,...,P_num_proc}
        vec_loc = (int*)malloc((num_loc) * sizeof(int)); //Allocation  vec_loc

        communication_tag = 10 * curr_id_proc;
        MPI_Recv(vec_loc, num_loc,MPI_INT, 0, communication_tag, MPI_COMM_WORLD, &mpi_status);
    }

    //For each process {P0,...,P_num_proc}
    int curr_partial_sum = calculate_partial_sum(vec_loc, &num_loc);


    switch (strategia) {
        case 1:
            strategy1(&curr_id_proc, &num_proc, curr_partial_sum, &mpi_status);
            break;
        case 2:
            if(is_power_of_two(&num_proc)){
                strategy2(&curr_id_proc, &num_proc, curr_partial_sum, &mpi_status);
            }
            else{
                strategy1(&curr_id_proc, &num_proc, curr_partial_sum, &mpi_status);
            }
            break;
        case 3:
            if(is_power_of_two(&num_proc)){
                strategy3(&curr_id_proc, &num_proc, curr_partial_sum, &mpi_status);
            }
            else{
                strategy1(&curr_id_proc, &num_proc, curr_partial_sum, &mpi_status);
            }
            break;
        default:
            printf("Strategia non valida!\n");
            break;
    }

    /*******  End MPI Program *******/
    MPI_Finalize();


    return 0;
}

/* ****************************************************************************************************************** */
int* read_input_numbers(int* punt_vec, int* num, char** punt_argv){
    for(int i = 0; i < (*num); i++) {
        *(punt_vec + i) = atoi(*(punt_argv + i + 3)); //In 3 third position of argv[] we find the elements number
    }
    return punt_vec;
}

int* generate_random_numbers(int *punt_vec, int* num){
    int const max = 1000;
    int const min = 1;

    srand(time(NULL));

    for(int i = 0; i < (*num); i++) {
        *(punt_vec + i) = (rand() % (max - min + 1)) + min;
    }
    return punt_vec;
}

/* ****************************************************************************************************************** */

bool is_power_of_two(int* num_proc){
    //All power of two numbers have only one bit set
    //If num is a power of 2 then bitwise & of num and num-1 will be zero
    return ((*num_proc != 0) && ((*num_proc & (*num_proc - 1)) == 0));
}

/* ****************************************************************************************************************** */

void print_vector(int* vec, int* num){
    printf("[");
    for(int i = 0; i < *num; i++){
        if(i == (*num)-1){
            printf("%d]\n");
            fflush(stdout);
        }
        else{
            printf("%d,", *(vec+i));
            fflush(stdout);
        }
    }
}

void sum_vector(int* vec, int* num){
    int sum = 0;
    for(int i = 0; i < *num; i++){
        sum += *(vec + i);
    }
    printf("@Vector SUM: %d\n", sum);
    fflush(stdout);
}

/* ****************************************************************************************************************** */

int calculate_partial_sum(int* punt_vec, int* num){
    int sum = 0;
    for(int i = 0; i < *num; i++){
        sum += *(punt_vec + i);
    }
    return sum;
}

/* ****************************************************************************************************************** */

void strategy1(int* curr_id_proc, int* num_proc, int partial_sum, MPI_Status* mpi_status){
    int tag_send, tag_receive;

    if((*curr_id_proc) == 0){
        //Start from partial_sum of P0
        int curr_sum = partial_sum;

        //P0 receive the partial sum for each P1,...,P_num_proc}
        for(int id = 1; id < (*num_proc); id++){
            tag_receive = 100 * id;
            MPI_Recv(&partial_sum, 1, MPI_INT, id, (100 * id), MPI_COMM_WORLD, mpi_status);
            curr_sum += partial_sum;
        }
        printf("@Strategy1 SUM: %d\n", curr_sum);
        fflush(stdout);
    }
    else{
        //All {P1,...,P_num_proc} processes send partial_sum to P0 process
        tag_receive = 100 * (*curr_id_proc);
        MPI_Send(&partial_sum, 1, MPI_INT, 0, tag_receive, MPI_COMM_WORLD);
    }
}

/* ****************************************************************************************************************** */
void strategy2(int* curr_id_proc, int* num_proc, int partial_sum, MPI_Status* mpi_status){
    int rec_sum = 0;
    int tag_send, tag_receive;

    for(int i = 0; i < log(*num_proc); i++){

        //Check if current process participates in the communication
        if(((*curr_id_proc) % (int)pow(2,i)) == 0){

            if(((*curr_id_proc) % (int)pow(2,i+1)) == 0){ //Current process is a receiver from curr_id_proc + 2^i
                tag_receive = 200 * i;
                MPI_Recv(&rec_sum, 1, MPI_INT, (*curr_id_proc + (int)pow(2, i)), tag_receive, MPI_COMM_WORLD, mpi_status);

                partial_sum += rec_sum;

            }
            else{ //Current process is a sender to curr_id_proc - 2^i
                tag_send = 200 * i;
                MPI_Send(&partial_sum, 1, MPI_INT, (*curr_id_proc - (int)pow(2,i)), tag_send , MPI_COMM_WORLD);
            }
        }
    }
    if(*curr_id_proc == 0){
        printf("@Strategy2 SUM: %d\n", partial_sum);
        fflush(stdout);
    }
}

/* ****************************************************************************************************************** */

void strategy3(int* curr_id_proc, int* num_proc, int partial_sum, MPI_Status* mpi_status){
    int rec_sum = 0;
    int tag_rec, tag_send;

    //All process participates in the communication
    for(int i = 0; i < log(*num_proc); i++){

        if(((*curr_id_proc) % (int)pow(2,i+1)) < (int)pow(2,i)){ //Current process is a receiver from curr_id_proc + 2^i

            tag_send = 400 * i;
            MPI_Send(&partial_sum, 1, MPI_INT, (*curr_id_proc + (int)pow(2,i)), tag_send , MPI_COMM_WORLD);

            tag_rec = 300 * i;
            MPI_Recv(&rec_sum, 1, MPI_INT, (*curr_id_proc + (int)pow(2, i)), tag_rec, MPI_COMM_WORLD, mpi_status);


            partial_sum += rec_sum;

        }
        else{ //Current process is a sender to curr_id_proc - 2^i

            tag_send = 300 * i;
            MPI_Send(&partial_sum, 1, MPI_INT, (*curr_id_proc - (int)pow(2,i)), tag_send , MPI_COMM_WORLD);

            tag_rec = 400 * i;
            MPI_Recv(&rec_sum, 1, MPI_INT, (*curr_id_proc - (int)pow(2, i)), tag_rec, MPI_COMM_WORLD, mpi_status);

            partial_sum += rec_sum;
        }
    }

    printf("P%d @Strategy3 SUM: %d\n", *curr_id_proc, partial_sum);
    fflush(stdout);
}
