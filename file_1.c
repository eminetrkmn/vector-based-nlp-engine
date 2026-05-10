#include <stdio.h>
#include <string.h>
char all_letters[100];
void write_vector(FILE *f,char *str,int max_len,int vector_size)
{
    char temp[1000];
    char temp1[1000];
    int i,j;
    int len = strlen(str);
    for(i=0;i<max_len-len;i++) 
    {
        for(j=0;j<vector_size;j++)
        {
            fprintf(f,"0");
        }
    }
    for(i=0;i<len;i++)
    {
        FILE *p;
        p = fopen("letters.txt","r");
        if(p == NULL)
        {
            printf("Could not open letters.txt file!\n");
            return;
        }
        int found = 0;
        while(fgets(temp,sizeof(temp),p) != NULL)
        {
            temp[strcspn(temp,"\n")] = '\0';
            if (temp[0] == str[i] && temp[1] == ':')
            {
                strcpy(temp1,temp+2);
                fprintf(f,"%s",temp1);
                found = 1;
                break;
            }
        }
        fclose(p);
        if(!found)
        {
            for(j = 0; j < vector_size; j++)
            {
                fprintf(f, "0");
            }
        }
    }
}
void sort_alphabetically(char str[])
{
    int i,j;
    int length = strlen(str);
    for(i=0;i<length-1;i++)
    {
        for(j=0;j<length-i-1;j++)
        {
            if(str[j]>str[j+1])
            {
                int temp = str[j];
                str[j] = str[j+1];
                str[j+1] = temp;
            }
        }
    }
}
void onehot_vector(char str[])
{
    int i,j;
    int length = strlen(str);
    FILE *ptr;
    ptr = fopen("letters.txt","w");
    for(i=0;i<length;i++)
    {
        fprintf(ptr,"%c:",str[i]);
        for(j=0;j<length;j++)
        {
            if(i==j)
            {
                fprintf(ptr,"1");
            }
            else
            {
                fprintf(ptr,"0");
            }
        }
        fprintf(ptr,"\n");
    }
    fclose(ptr);
}
void my_alphabet(char array[])
{
    int i,j,control;
    for(i=0;array[i] != '\0';i++)
    {
        control=1;
        for(j=0;all_letters[j] != '\0';j++)
        {
            if(array[i] == all_letters[j])
            {
                control=0;
                break;
            }
        }
        if(control == 1)
        {
            all_letters[j] = array[i];
            all_letters[j + 1] = '\0';
        }
    }
}
void divide_letters(char *str)
{
    int i;
    char letters[1000];
    for(i=0;str[i] != '\0';i++)
    {
        letters[i] = str[i];
    }
    letters[i] = '\0';
    my_alphabet(letters);
}
int main()
{
    int i=0,j;
    char str[1000];
    char questions[1000][1000];
    char temp_q[1000];
    char answers[1000][1000];
    char temp_a[1000];
    all_letters[0] = '\0';
    FILE *p;
    p = fopen("database.txt","r");
    if(p == NULL)
    {
        printf("File could not be opened\n");
        return 1;
    }

    while(fgets(str,sizeof(str),p) != NULL)
    {
        str[strcspn(str, "\r\n")] = '\0';
        if(strncmp(str,"Q:",2) == 0)
        {
            strcpy(questions[i],str+2);
        }
        else if(strncmp(str,"A:",2) == 0)
        {
            strcpy(answers[i],str+2);
        }
        else if(strcmp(str,"---") == 0)
        {
            i++;
        }
    }
    int qa = i+1;
    fclose(p);
    for(j=0;j<qa;j++)
    {
        strcpy(temp_q,questions[j]);
        char *token = strtok(temp_q," ");
        while(token != NULL)
        {
            divide_letters(token);
            token = strtok(NULL," ");
        }

        strcpy(temp_a,answers[j]);
        char *token1 = strtok(temp_a," ");
        while(token1 != NULL)
        {
            divide_letters(token1);
            token1 = strtok(NULL," ");
        }
    }
    sort_alphabetically(all_letters);
    onehot_vector(all_letters);
    int vector_size = strlen(all_letters);
    int max_len = 0;
    for(i=0;i<qa;i++)
    {
        int q_len = strlen(questions[i]);
        int a_len = strlen(answers[i]);
        if(max_len<q_len)
        max_len = q_len;
        if(max_len<a_len)
        max_len = a_len;
    }
    
    FILE *ptr;
    ptr = fopen("embeddings.txt","w");
    fprintf(ptr,"Metadata Part\n");
    fprintf(ptr,"The maximum length of a sentence embedding across all questions and answers:%d\n",max_len);
    fprintf(ptr,"The dimension of the word embeddings:%d\n",vector_size);
    fprintf(ptr,"The number of question-answer pairs:%d\n",qa);
    fprintf(ptr,"#####\n");
    for(i=0;i<qa;i++)
    {
        fprintf(ptr,"Question: ");
        write_vector(ptr,questions[i],max_len,vector_size);
        fprintf(ptr,"\n");
        fprintf(ptr,"Answer: ");
        write_vector(ptr,answers[i],max_len,vector_size);
        fprintf(ptr,"\n");
        fprintf(ptr,"---\n");
    }
    fclose(ptr);
    return 0;
}
