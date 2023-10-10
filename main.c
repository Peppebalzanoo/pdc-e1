#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include "mpi.h"


int* read_input_numbers(int* punt_vec, int* num);

int* generate_random_numbers(int* punt_vec, int* num);

int calculate_partial_sum(int* punt_vec, int* num);

void print_vector(int* vec, int* num);
/* ************************************************************************* */

int main(int argc, char *argv[]) {
    int curr_id_proc, num_proc;
    int num_elem, num_loc, num_rest;
    int count_elem, index;
    int *vec = NULL, *vec_loc = NULL;

    int communication_tag = 0;
    MPI_Status mpi_status;

    MPI_Init(&argc, &argv);

    /*******  Start MPI Program *******/
    MPI_Comm_rank(MPI_COMM_WORLD, &curr_id_proc);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);


    /* Process P0 read (num_elem, vec: vector_of_elements) and send information at all process */
    if(curr_id_proc == 0){
        num_elem = atoi(argv[1]);

        //Allocate memory for the input elements
        vec = (int*)malloc((num_elem) * sizeof(int));

        if (num_elem <= 20){
            vec = read_input_numbers(vec, &num_elem);
            print_vector(vec, &num_elem);
        }
        else{
            //Allocate memory for the input elements
            vec = generate_random_numbers(vec, &num_elem);
            print_vector(vec, &num_elem);
        }
    }
    MPI_Bcast(&num_elem, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //All processes calculate num_loc and num_rest
    num_loc = (num_elem / num_proc);  // Number of elements for current process
    num_rest = (num_elem % num_proc); // Number of elements to assign at {P0,...,(P_num_rest - 1)} processes
    printf("2\n");
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
            communication_tag = 100 * id;

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

        communication_tag = 100 * curr_id_proc;
        MPI_Recv(vec_loc, num_loc,MPI_INT, 0, communication_tag, MPI_COMM_WORLD, &mpi_status);
    }

    //For each process {P0,...,P_num_proc}
    int curr_sum = calculate_partial_sum(vec_loc, &num_loc);

    if((num_proc != 0) && ((num_proc & (num_proc - 1)) == 0)){
        printf("Ci sono numero processori potenza di 2");
        // strategia1();
        // strategia2();
        // strategia3();
    }else{
        printf("Non ci sono numero processori potenza di 2");
        // strategia2();
        // strategia1();
    }

    /*******  End MPI Program *******/
    MPI_Finalize();


    return 0;
}

/* ************************************************************************* */
int* read_input_numbers(int* punt_vec, int* num){
    for(int i = 0; i < (*num); i++) {
        *(punt_vec + i) = 10 * i;
    }
    return punt_vec;
}


int* generate_random_numbers(int *punt_vec, int* num){
    return read_input_numbers(punt_vec, num);
}

bool is_power_of_two(){

}
/* ************************************************************************* */

int calculate_partial_sum(int* punt_vec, int* num){
    int sum = 0;
    for(int i = 0; i < *num; i++){
        sum += *(punt_vec + i);
    }
    return sum;
}
/* ************************************************************************* */
void print_vector(int* vec, int* num){
    for(int i = 0; i < *num; i++){
        printf("\nvec[%d]: %d", i, *(vec+i));
        fflush(stdout);
    }
}
/* ************************************************************************* */