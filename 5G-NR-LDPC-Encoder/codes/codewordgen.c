#include <stdio.h>
#include <stdlib.h>
#include<math.h>
void exor(int temp[],int y[],int Zc)
{
    for(int i = 0;i<Zc;i++){
    temp[i] = temp[i] +y[i];
    temp[i] = temp[i]%2;
    }
}
void slice(int msg[],int l,int u,int x[])
{
    int m = 0;
    for(int i = l ; i <= u ; i++)
    {
        x[m] = msg[i];
        m++;
    }
}
void mul_sh(int x[], int k, int y[],int Zc)
{
if (k == -1)
{
for (int i = 0; i < Zc; i++)
y[i] = 0;
}
else
{
for (int i = 0; i < Zc; i++)
y[i] = x[(i + k) % Zc];
}
}
void ldpc(int bg_no, int iLs, int Zc,int msg[])
{
    //Loading base graph from text file to array : B
    ///////////////////////////////////////////////////////////////
    int **base_graph_raw;
    int m = 42,n = 52;
    
    char filename[14];
    int **B;
    sprintf(filename,"BG_%d_%d_%d.txt",bg_no,iLs,Zc);
    printf("%s\n",filename);
    base_graph_raw = malloc(sizeof(int *) * m);
    for(int i = 0;i<m;i++)
    base_graph_raw[i] = malloc(sizeof(int) * n);

    B = malloc(sizeof(int *) * m);
    for(int i = 0;i<m;i++)
    B[i] = malloc(sizeof(int) * n);
    FILE *file;
    file = fopen(filename,"r");

    if(file == NULL)
    {
        printf("error opening the file");
        //return 1;
    }

    int row = 0;
    while(!feof(file))
    {
        if(ferror(file))
        {
            printf("Error opening the file");
            //return 1;
        }

        for(int i = 0; i< n;i++)
        {
           if(fscanf(file,"%d", &base_graph_raw[row][i]) == EOF)
           break;
        }
        row++;
        if(row == m) break;
    }

    for(int i = 0; i< m; i++)
    {
        for(int j = 0;j<n;j++)
        {
            B[i][j] = base_graph_raw[i][j] % Zc;
        }
    }
    //////////////////////////////////////////////////
    //******LDPC Encoding******
    //iterators
    int i,j,p,f;
    //cword : codeword ; temp : to store modulo sum
    printf("The msg generated is \n");
    for(i = 0;i<40;i++)
    printf("%d ",msg[i]);
    int cword_len = n*Zc,msg_len= (n-m)*Zc;
    int cword[cword_len],temp[Zc];
    printf("\n");
    for(i = 0; i< n*Zc;i++)
    {
        cword[i] = 0;
    }
    printf("\n");
    for(i=0 ; i<msg_len;i++)
    cword[i] = msg[i];
    
    for(i = 0;i<Zc;i++)
    temp[i] = 0;

    printf("\n");
    for(i=0;i<4;i++)
    {
        int y[Zc],x[Zc];
        for(j = 0; j < n-m ;j++){
        slice(msg,j*Zc,((j+1)*Zc-1),x);
        mul_sh(x,B[i][j],y,Zc);
        exor(temp,y,Zc);
        }
    }

    printf("\n");
    int p1_sh,x[Zc],y[Zc];
    if(B[1][n-m] == -1)
    p1_sh = B[2][n-m];
    else
    p1_sh = B[2][n-m+1];
    
    //slice(cword,(n-m)*Zc,(n-m+1)*Zc - 1,x);
    mul_sh(temp,Zc-p1_sh,y,Zc);
    p=0;
    for(f = (n-m)*Zc;f<=(n-m+1)*Zc-1; f++)
    {
        cword[f] = y[p];
        p++;
    }
    
    for(i =0; i<3;i++)
    {
        for(p = 0;p<Zc;p++)
        temp[p] = 0;
        for(j = 0;j<=n-m+i;j++)
        {
            slice(cword,j*Zc,(j+1)*Zc-1,x);
            mul_sh(x,B[i][j],y,Zc);
            exor(temp,y,Zc);
        }
        p = 0;
        for(f = (n-m+i+1)*Zc;f<=(n-m+i+2)*Zc-1;f++)
        {
            
            cword[f] = temp[p];
            p++;
        }
    }
    for(i =4; i<m;i++)
    {
        for(p = 0;p<Zc;p++)
        temp[p] = 0;
        for(j = 0;j<=n-m+3;j++)
        {
            slice(cword,j*Zc,(j+1)*Zc-1,x);
            mul_sh(x,B[i][j],y,Zc);
            exor(temp,y,Zc);
        }
        p = 0;
        for(f = (n-m+i)*Zc;(f<=(n-m+i+1)*Zc-1 && p<Zc);f++)
        {
            cword[f] = temp[p];
            p++;
        }
        
    }
    
    printf("The code word is \n");
    for(i = 0;i<cword_len;i++)
    {
        printf("%d ",cword[i]);
        
    }
}

int main(void)
{
int K,N,a,Zc,j,i, iLs,bg_no;
float y,Kb,R;
printf("Enter the block size(K):");
scanf("%d",&K);
printf("Enter length of message(N):");
scanf("%d",&N);
R=K/N;
printf("code rate R:%f\n",R);

//Selection
if(K>640 && K<=3824)
{
    Kb=10;
    printf("Kb=%.1f\n",Kb);
}
else if(K>560 && K<=640)
{
    Kb=9;
    printf("Kb=%.1f\n",Kb);
}
else if(K>192 && K<=560)
{
    Kb=8;
    printf("Kb=%.1f\n",Kb);
}
else if( K<=192)
{
    Kb=6;
    printf("Kb=%.1f\n",Kb);
}
else
{
     Kb=22;
    printf("Kb=%.1f\n",Kb);
}
y=K/Kb;
printf("Zc>=%f\n",y);

//Set Index(iLs) & Lifting size(Zc)
int iL_s[8][8] =  {   {2, 	4,	8,	16,	32,	64,	128,	256},
            {3, 	6,	12,	24,	48,	96,	192,	384},
            {5, 	10,	20,	40,	80,	160,	320,	0  },
            {7, 	14,	28,	56,	112,	224,	0,  	0  },
            {9, 	18,	36,	72,	144,	288,	0,  	0  },
            {11,	22,	44,	88,	176,	352,	0,  	0  },
            {13,	26,	52,	104,	208,	0,	0,  	0  },
            {15,	30,	60,	120,	240,	0,	0,  	0  }  };
            
int r[100];
int k=0;
for (i=0;i<8;i++)
    {
        for (j=0;j<8;j++)
        {
          if( iL_s[i][j] > y)
          {
            r[k] = iL_s[i][j];
           // printf(" %d ",r[k]);
            k++;
          }  
            
        }
        
    }
 Zc=r[0];
    for (i=0; i<k; i++)
    {
        if (r[i] < Zc)
        {
            Zc = r[i];
        }
    }
   
printf("So we consider nearer available Zc:%d\n",Zc);
    
   for (i=0;i<8;i++)
    {
        for (j=0;j<8;j++)
        {
            
             if(Zc!=iL_s[i][j])
               {
                 continue;
               }
        
             else
              {
                  iLs=i;
                printf("\nSet Index 'iLs'=%d\n",iLs);
              }
        }
        
    }

    if((K<3824&& R<=0.67)||(K<=292) || (R<=0.25))
    {
        printf("BG2 is selected\n");
        bg_no = 2;
    }
    else{
        printf("BG1 is selected");
        bg_no = 1;
    }
    int msg[N];
    // generate random binary array
    for (int i = 0; i < N; i++) {
        msg[i] = rand() % 2;// generate either 0 or 1 randomly
    }
    ldpc(bg_no,iLs,Zc,msg);
    return 0;   
}
