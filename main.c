#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "mpi.h"


int* read_input_numbers(int* punt_vec, const int* num, char** punt_argv);

int* generate_random_numbers(int* punt_vec, const int* num);

bool is_power_of_two(const int* num_proc);

int calculate_partial_sum(const int* punt_vec, const int* num);

int* calculate_pow(int* punt_pow, const int* num);

void print_vector(const int* vec, const int* num);

void sum_vector(const int* vec, const int* num);

void strategy1(const int* curr_id_proc, const int* num_proc, int partial_sum,  MPI_Status* mpi_status);

void strategy2(const int* curr_id_proc, const int* num_proc, const double* log_proc, int partial_sum, int* punt_pow, MPI_Status* mpi_status);

void strategy3(const int* curr_id_proc, const int* num_proc, const double* log_proc, int partial_sum, int* punt_pow, MPI_Status* mpi_status);

/* ****************************************************************************************************************** */

int main(int argc, char** argv) {
    int curr_id_proc, num_proc, num_input_elem, num_loc, num_rest, count_elem, index;
    double log_proc;
    int *vec = NULL, *vec_loc = NULL, *pow = NULL;
    int communication_tag = 0;
    MPI_Status mpi_status;

    int strategia = atoi(*(argv+1));

    MPI_Init(&argc, &argv);

    /*******  Start MPI Program *******/
    MPI_Comm_rank(MPI_COMM_WORLD, &curr_id_proc);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    //Each process calculate the array of pow
    pow = (int*)malloc((num_proc) * sizeof(int));
    calculate_pow(pow, &num_proc);

    // Process P0 read (num_input_elem, vec: vector_of_elements) and send information at all process
    if(curr_id_proc == 0){
        num_input_elem = atoi(*(argv + 2)); //In third position of argv[] we find the number of elements

        //Allocate memory for the input elements
        vec = (int*)malloc((num_input_elem) * sizeof(int));

        if (num_input_elem <= 20){
            vec = read_input_numbers(vec, &num_input_elem, argv);
        }
        else{
            vec = generate_random_numbers(vec, &num_input_elem);
        }
        print_vector(vec, &num_input_elem);
        sum_vector(vec, &num_input_elem);
    }
    MPI_Bcast(&num_input_elem, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //All processes calculate num_loc and num_rest
    num_loc = (num_input_elem / num_proc);  // Number of elements for current process
    num_rest = (num_input_elem % num_proc); // Number of elements to assign at {P0,...,(P_num_rest - 1)} processes

    log_proc = log2(num_proc);

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
    printf("P%d [%d elements]: %d\n", curr_id_proc, num_loc, curr_partial_sum);
    fflush(stdout);


    switch (strategia) {
        case 1:
            strategy1(&curr_id_proc, &num_proc, curr_partial_sum, &mpi_status);
            break;
        case 2:
            if(is_power_of_two(&num_proc)){
                strategy2(&curr_id_proc, &num_proc, &log_proc, curr_partial_sum, pow, &mpi_status);
            }
            else{
                strategy1(&curr_id_proc, &num_proc, curr_partial_sum, &mpi_status);
            }
            break;
        case 3:
            if(is_power_of_two(&num_proc)){
                strategy3(&curr_id_proc, &num_proc, &log_proc, curr_partial_sum, pow, &mpi_status);
            }
            else{
                strategy1(&curr_id_proc, &num_proc, curr_partial_sum,  &mpi_status);
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
int* read_input_numbers(int* const punt_vec, const int* num, char** const punt_argv){
    for(int i = 0; i < (*num); i++) {
        *(punt_vec + i) = atoi(*(punt_argv + i + 3)); //In 3 third position of argv[] we find the elements number
    }
    return punt_vec;
}

int* generate_random_numbers(int *punt_vec, const int* num){
    int const max = 1000;
    int const min = 1;

    srand(time(NULL));

    for(int i = 0; i < (*num); i++) {
        *(punt_vec + i) = (rand() % (max - min + 1)) + min;
    }
    return punt_vec;
}

/* ****************************************************************************************************************** */

bool is_power_of_two(const int* num_proc){
    //All power of two numbers have only one bit set
    //If num is a power of 2 then bitwise & of num and num-1 will be zero
    return ((*num_proc != 0) && ((*num_proc & (*num_proc - 1)) == 0));
}

/* ****************************************************************************************************************** */

void print_vector(const int* vec, const int* num){
    printf("[");
    for(int i = 0; i < *num; i++){
        if(i == (*num)-1){
            printf("%d]\n", *(vec+i));
            fflush(stdout);
        }
        else{
            printf("%d,", *(vec+i));
            fflush(stdout);
        }
    }
}

void sum_vector(const int* vec, const int* num){
    int sum = 0;
    for(int i = 0; i < *num; i++){
        sum += *(vec + i);
    }
    printf("\n@Vector SUM: %d\n\n", sum);
    fflush(stdout);
}

/* ****************************************************************************************************************** */

int calculate_partial_sum(const int* punt_vec, const int* num){
    int sum = 0;
    for(int i = 0; i < *num; i++){
        sum += *(punt_vec + i);
    }
    return sum;
}

int* calculate_pow(int* punt_pow, const int* num){
    *(punt_pow) = 1;
    int pow = 2;
    for(int i = 1; i < *num; i++){
        *(punt_pow + i) = pow;
        pow *= 2;
    }
    return punt_pow;
}
/* ****************************************************************************************************************** */

void strategy1(const int* curr_id_proc, const int* num_proc, int partial_sum, MPI_Status* mpi_status){
    int tag_receive;

    if((*curr_id_proc) == 0){
        //Start from partial_sum of P0
        int curr_sum = partial_sum;

        //P0 receive the partial sum for each P1,...,P_num_proc}
        for(int id = 1; id < (*num_proc); id++){

            tag_receive = (100 * id);
            MPI_Recv(&partial_sum, 1, MPI_INT, id, tag_receive, MPI_COMM_WORLD, mpi_status);
            curr_sum += partial_sum;
        }
        //Process P0 print the total sum
        printf("\n#P0 Strategy1 SUM: %d\n", curr_sum);
        fflush(stdout);
    }
    else{
        //All {P1,...,P_num_proc} processes send partial_sum to P0 process
        tag_receive = (100 * (*curr_id_proc));
        MPI_Send(&partial_sum, 1, MPI_INT, 0, tag_receive, MPI_COMM_WORLD);
    }
}

/* ****************************************************************************************************************** */
void strategy2(const int* curr_id_proc, const int* num_proc, const double* log_proc, int partial_sum, int* punt_pow, MPI_Status* mpi_status){
    int rec_sum = 0;
    int tag_send, tag_receive;

    for(int i = 0; i < (*log_proc); i++){

        //Check if current process participates in the communication
        if((*curr_id_proc % *(punt_pow+i)) == 0){

            if((*curr_id_proc % *(punt_pow+i+1)) == 0){ //Current process is a receiver from curr_id_proc + 2^i
                tag_receive = 200 * i;
                MPI_Recv(&rec_sum, 1, MPI_INT, (*curr_id_proc + *(punt_pow+i)), tag_receive, MPI_COMM_WORLD, mpi_status);

                partial_sum += rec_sum;

            }
            else{ //Current process is a sender to curr_id_proc - 2^i
                tag_send = 200 * i;
                MPI_Send(&partial_sum, 1, MPI_INT, (*curr_id_proc - *(punt_pow+i)), tag_send , MPI_COMM_WORLD);
            }
        }
    }
    if(*curr_id_proc == 0){
        printf("\n#P0 Strategy2 SUM: %d\n", partial_sum);
        fflush(stdout);
    }
}

/* ****************************************************************************************************************** */

void strategy3(const int* curr_id_proc, const int* num_proc, const double* log_proc, int partial_sum, int* punt_pow, MPI_Status* mpi_status){
    int rec_sum = 0;
    int tag_rec, tag_send;

    //All process participates in the communication
    for(int i = 0; i < (*log_proc); i++){
        if( (*curr_id_proc % *(punt_pow + i + 1) ) < *(punt_pow + i) ){ //Current process is a sender/receiver from curr_id_proc + 2^i
            tag_send = 400 * i;
            MPI_Send(&partial_sum, 1, MPI_INT, (*curr_id_proc + *(punt_pow + i)), tag_send , MPI_COMM_WORLD);


            tag_rec = 300 * i;
            MPI_Recv(&rec_sum, 1, MPI_INT, (*curr_id_proc + *(punt_pow + i)), tag_rec, MPI_COMM_WORLD, mpi_status);

            partial_sum += rec_sum;

        }
        else{ //Current process is a sender/receiver to curr_id_proc - 2^i
            tag_send = 300 * i;
            MPI_Send(&partial_sum, 1, MPI_INT, (*curr_id_proc - *(punt_pow + i)), tag_send , MPI_COMM_WORLD);

            tag_rec = 400 * i;
            MPI_Recv(&rec_sum, 1, MPI_INT, (*curr_id_proc - *(punt_pow + i)), tag_rec, MPI_COMM_WORLD, mpi_status);

            partial_sum += rec_sum;
        }
    }

    printf("\n#P%d Strategy3 SUM: %d\n", *curr_id_proc, partial_sum);
    fflush(stdout);
}
