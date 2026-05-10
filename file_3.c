#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define stack_size 3
#define stack_string 10

char stack[stack_size][stack_string];
int top=0;
int isEmpty()
{
    return top==0;
}
void push(char *operation)
{
    if(top == stack_size)
    {
        printf("Stack is full! Could not add element.\n");
    }
    else if(top < stack_size)
    {
        strcpy(stack[top],operation);
        top++;
    }
}
void pop(char *operation)
{
    if(isEmpty())
    {
        printf("Stack is empty! Could not remove element.\n");
    }
    else if(top>0)
    {
        top--;
        strcpy(operation,stack[top]);
    }
}

enum Color
{
    RGB,
    BGR,
    GRAY
};

void print_output_file(int* results2_1,int* results3_1,int* results4_1,int* results2_2,int* results3_2,int* results4_2,int* results2_3,int* results3_3,int* results4_3,enum Color color,int length1,int length2)
{
    int result_size = length2 - length1 + 1,i;
    FILE *ptr;
    ptr=fopen("output.txt","w");
    if(color == RGB)
    {
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"[%d,%d,%d]",results2_1[i],results3_1[i],results4_1[i]);
            if(i < result_size-1)
            fprintf(ptr,",");
        }
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"[%d,%d,%d]",results2_2[i],results3_2[i],results4_2[i]);
            if(i < result_size-1)
            fprintf(ptr,",");
        }
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"[%d,%d,%d]",results2_3[i],results3_3[i],results4_3[i]);
            if(i < result_size-1)
            fprintf(ptr,",");
        }
    }
    else if(color == BGR)
    {
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"[%d,%d,%d]",results4_1[i],results3_1[i],results2_1[i]);
            if(i < result_size-1)
            fprintf(ptr,",");
        }
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"[%d,%d,%d]",results4_2[i],results3_2[i],results2_2[i]);
            if(i < result_size-1)
            fprintf(ptr,",");
        }
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"[%d,%d,%d]",results4_3[i],results3_3[i],results2_3[i]);
            if(i < result_size-1)
            fprintf(ptr,",");
        }
    }
    else if(color == GRAY)
    {
        fprintf(ptr,"[");
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"%d,",results2_1[i]);
        }
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"%d,",results2_2[i]);
        }
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"%d",results2_3[i]);
            if(i < result_size-1)
            fprintf(ptr,",");
        }
        fprintf(ptr,"]");
        fprintf(ptr,"\n");
        fprintf(ptr,"[");
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"%d,",results3_1[i]);
        }
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"%d,",results3_2[i]);
        }
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"%d",results3_3[i]);
            if(i < result_size-1)
            fprintf(ptr,",");
        }
        fprintf(ptr,"]");
        fprintf(ptr,"\n");
        fprintf(ptr,"[");
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"%d,",results4_1[i]);
        }
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"%d,",results4_2[i]);
        }
        for(i=0;i<result_size;i++)
        {
            fprintf(ptr,"%d",results4_3[i]);
            if(i < result_size-1)
            fprintf(ptr,",");
        }
        fprintf(ptr,"]");
    }
    fclose(ptr);
}
int max_operation(int* chunk, int* corresponding_chunk_part, int chunk_size)
{
    int i,max;
    int *results = (int*)malloc(chunk_size*sizeof(int));
    for(i=0;i<chunk_size;i++)
    {
        results[i] = chunk[i]*corresponding_chunk_part[i];
    }
    max=results[0];
    for(i=1;i<chunk_size;i++)
    {
        if(results[i]>max)
        {
            max=results[i];
        }
    }
    free (results);
    return max;
}
int avg_operation(int* chunk, int* corresponding_chunk_part, int chunk_size)
{
    int i,result=0;
    for(i=0;i<chunk_size;i++)
    {
        result += chunk[i]*corresponding_chunk_part[i];
    }
    return result/chunk_size;
}
int* calculate_results(int* chunk, int* line, int chunk_size, int (*operation)(int*, int*, int), int line_size)
{
    int i,j,k;
    int *array = (int*)malloc((line_size - chunk_size + 1)*sizeof(int));
    int *corresponding_chunk_part = (int*)malloc(chunk_size*sizeof(int));

    for(i=0;i<line_size-chunk_size+1;i++)
    {
        for(k=i,j=0;j<chunk_size;j++,k++)
        {
            corresponding_chunk_part[j] = line[k];
        }
        array[i]=operation(chunk,corresponding_chunk_part,chunk_size);
    }
    free(corresponding_chunk_part);
    return array;
}
int main()
{
    int i=0,j,k,length1,length2,length3,length4;
    int corresponding_chunk_part[85],chunk[10];
    char input[stack_string];
    while(top<stack_size)
    {
        printf("Enter transaction (max/avg) ");
        scanf("%s",input);
        if(strcmp(input,"max") == 0 || strcmp(input,"avg") == 0)
        {
            push(input);
        }
        else
        {
            printf("Invalid operation! Only 'max' or 'avg' is allowed.\n");
        }
    }

    enum Color color;
    printf("Please select color mode (rgb / bgr / gray): ");
    scanf("%s", input);
    if(strcmp(input,"rgb") == 0)
    {
        color = RGB;
    }
    else if(strcmp(input,"bgr") == 0)
    {
        color = BGR;
    }
    else if(strcmp(input,"gray") == 0)
    {
        color = GRAY;
    }
    else
    {
        printf("Invalid selection!\n");
        return 1;
    }

    FILE *ptr;
    ptr=fopen("input.txt","r");
    if(ptr== NULL)
    return 1;
    int line1[10],line2[85],line3[85],line4[85];
    char array1[100]="",array2[100]="",array3[100]="",array4[100]="";
    fgets(array1,sizeof(array1),ptr);
    fgets(array2,sizeof(array2),ptr);
    fgets(array3,sizeof(array3),ptr);
    fgets(array4,sizeof(array4),ptr);
    i=0;
    char *num = strtok(array1," ");
    while(num != NULL)
    {
        line1[i] = atoi(num);
        num=strtok(NULL," ");
        i++;
    }
    length1=i;
    i=0;
    char *num1 = strtok(array2," ");
    while(num1 != NULL)
    {
        line2[i] = atoi(num1);
        num1=strtok(NULL," ");
        i++;
    }
    length2=i;
    i=0;
    char *num2 = strtok(array3," ");
    while(num2 != NULL)
    {
        line3[i] = atoi(num2);
        num2=strtok(NULL," ");
        i++;
    }
    length3=i;
    i=0;
    char *num3 = strtok(array4," ");
    while(num3 != NULL)
    {
        line4[i] = atoi(num3);
        num3=strtok(NULL," ");
        i++;
    }
    length4=i;
    fclose(ptr);

    int *results2_1, *results3_1, *results4_1;
    int *results2_2, *results3_2, *results4_2;
    int *results2_3, *results3_3, *results4_3;

    while(!isEmpty())
    {
        char operation[stack_string];
        pop(operation);
        int (*operation_function)(int*, int*, int);
        if(strcmp(operation,"max") == 0)
        {
            operation_function = max_operation;
        }
        else if(strcmp(operation,"avg") == 0)
        {
            operation_function = avg_operation;
        }
        if(top == 2)
        {
            results2_1 = calculate_results(line1,line2,length1,operation_function,length2);
            results3_1 = calculate_results(line1,line3,length1,operation_function,length3);
            results4_1 = calculate_results(line1,line4,length1,operation_function,length4);
        }
        else if(top == 1)
        {
            results2_2 = calculate_results(line1,line2,length1,operation_function,length2);
            results3_2 = calculate_results(line1,line3,length1,operation_function,length3);
            results4_2 = calculate_results(line1,line4,length1,operation_function,length4);
        }
        else if(top == 0)
        {
            results2_3 = calculate_results(line1,line2,length1,operation_function,length2);
            results3_3 = calculate_results(line1,line3,length1,operation_function,length3);
            results4_3 = calculate_results(line1,line4,length1,operation_function,length4);
        }
    }
    print_output_file(results2_1,results3_1,results4_1,results2_2,results3_2,results4_2,results2_3,results3_3,results4_3,color,length1,length2);
    printf("Results have been written to output.txt\n");
    free(results2_1);
    free(results3_1); 
    free(results4_1);
    free(results2_2); 
    free(results3_2); 
    free(results4_2);
    free(results2_3); 
    free(results3_3); 
    free(results4_3);
    return 0;
}