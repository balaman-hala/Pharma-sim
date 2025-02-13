#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

typedef struct {
    int day;
    int month;
    int year;
}date;

typedef struct {
    char name[100];
    int quantity;
    double price;
    date fxpDate;
    date expDate;
    char brand[100];
}med;


int exist( char name[100], char brand[100]){ //to check if a medicine already exists
    long pos = -1;
    med a;
    FILE *F = fopen("meds.bin","rb");
    if ( F == NULL ){
        printf(RED"Error opening file.\n"RESET);
    }
    while ( fread( &a, sizeof(med), 1, F ) == 1 ){
        if ( strcmp( name , a.name ) == 0 && strcmp( brand , a.brand ) == 0 ){
            pos = ftell(F) - sizeof(med);
            return pos;
        }
    }
    return pos; // if it exists return the position of the medicine in the file
}


void Date ( date *d ){
    int f = 1;
    do{
        printf("YYYY = ");
        scanf("%d",&d->year);
        if ( d->year <= 0 )
        {
            printf("INVALID YEAR TRY AGAIN \n");
        }
    } while ( d->year <= 0 );
    
    do{
        printf("MM = ");
        scanf("%d",&d->month);
        if ( d->month <= 0 || d->month > 12 ){
            printf(RED"INVALID MONTH TRY AGAIN \n"RESET);
        }
    } while ( d->month <= 0 || d->month > 12);
    
    do{
        printf("DD = ");
        scanf("%d",&d->day);
        if( d->day <= 0 || d->day > 31 )
        {
            printf(RED"INVALID DAY TRY AGAIN \n"RESET);
        }
        else{
            switch( d->month ){
                case 4 : case 6: case 8: case 10: case 12:
                if( d->day == 31 )
                printf(RED"INVALID DAY TRY AGAIN \n"RESET);
                else
                f = 0;
                break;
                case 2 :
                if ( d->day > 29 || (d->day == 29 && d->year % 4 != 0 ))
                printf(RED"INVALID DAY TRY AGAIN \n"RESET);
                else
                f = 0;
                break;
                default:
                f = 0;
                break;
            }
        }
    } while ( f == 1 );
    
    
}
int DateVerfication ( date d1 , date d2 ){ // to compare two dates
    if( d1.year > d2.year )
    return 0;

    else if ( d1.year == d2.year ){
        if ( d2.month < d1.month )
        return 0;

        else if ( d2.month == d1.month ){
            if ( d2.day < d1.day )
            return 0;
        }
    }

    return 1;
}
void warning ( date expirydate, date currentdate ){ // to warn the user if the expiry date of a medication is near
    if ( expirydate.year == currentdate.year && expirydate.month == currentdate.month && expirydate.day > currentdate.day ){
        int daysleft = expirydate.day - currentdate.day;
        char a;
        printf(RED"Warning: The expiry date of this medication is near. There are %d days left until expiry. Please check the expiry date before adding it to the stock.\n"RESET,daysleft);
    }
}
void Delete(FILE *F, int pos ){ // to delete a medicine from the stock
    F = fopen("meds.bin","rb");
    FILE *temp = fopen("temp.bin","wb");
    if ( F == NULL || temp == NULL ){
        printf("Error opening file.\n");
        exit(1);
    }
    med a;
    int i = 0;
    while ( i < pos ){
        fread( &a, sizeof(med), 1, F);
        fwrite(&a,sizeof(med),1,temp);
        i++;
    }
    if (fread(&a, sizeof(med), 1, F) != 1) {
        fclose(temp);
        fclose(F);
        remove("temp.bin");
        return; 
    }
    while ( fread( &a, sizeof(med), 1, F) == 1 ){
        fwrite(&a,sizeof(med),1,temp);
    }
    fclose(temp);fclose(F);
    remove("meds.bin");
    rename("temp.bin","meds.bin");
}


void UpdateQuantity(FILE *F, int pos, int newquan , int flag ){// to update the quantity of a medication
    F = fopen("meds.bin","rb+");
    med a;int choice,i = 0;
    if (flag){ // used the flag to know if the call of the function was from the menu or when adding the same medicine again
     printf("\n--------------------------------------------------------------------------------------------------\n\n");
    printf("How would you like to adjust the quantity?\n");
    printf("1. Decrease\n");
    printf("2. Increase\n");
    printf("3. Set new quantity\n");
     printf("\n--------------------------------------------------------------------------------------------------\n\n");
    do{
    printf(CYAN"Your choice : "RESET);
    scanf("%d",&choice);
    if ( choice == 1 ){ //decrease the quantity
        printf("Please enter the quantity you'd like to remove : ");
        scanf("%d",&newquan);
        while ( i <= pos ){
        fread( &a, sizeof(med), 1, F);
        i++;
        }
        if ( a.quantity - newquan < 0 ){
            printf(RED"Error: The quantity you'd like to remove is more than the quantity in stock.\n"RESET);
            return;
        }else if( a.quantity - newquan == 0 ){ //if the quantity reached 0 we delete the medication
            fclose(F);
            Delete(F,pos);
        }else{// else we just decrease the quantity
            a.quantity = a.quantity - newquan;
            fseek(F, -1 * sizeof(med), SEEK_CUR);
            fwrite( &a, sizeof(med), 1, F);
            printf(GREEN,"QUANTITY UPDATED \n"RESET);
        }
    }else if ( choice == 2 ){ //increase the quantity
        printf("Please enter the quantity you'd like to add : ");
        do{
            scanf("%d",&newquan);
            if ( newquan < 0 )
            printf(RED"Error: The quantity you'd like to add is less than 0.\n"RESET);
        }while ( newquan < 0 );
        while ( i <= pos ){
        fread( &a, sizeof(med), 1, F);
        i++;
        }
        a.quantity = a.quantity + newquan;
        fseek(F, -1 * sizeof(med), SEEK_CUR);
        fwrite( &a, sizeof(med), 1, F);
        printf(GREEN,"QUANTITY UPDATED \n"RESET);
    }else if ( choice == 3){ // change the quantity
        printf("Enter the new quantity :\n");
        do{
            scanf("%d",&newquan);
            if ( newquan < 0 ){
                printf(RED"Error: The quantity you'd like to add is less than 0.\n"RESET);
            }
        }while( newquan < 0 );
        while ( i <= pos ){
        fread( &a, sizeof(med), 1, F);
        i++;
        }
        if ( newquan == 0 ){ // if the new quantity is equal to 0 we remove the medication
            fclose(F);
            Delete(F,pos);
        }else{
            fseek(F, -1 * sizeof(med),SEEK_CUR); // to go back to the position of the medication
            a.quantity = newquan;
            fwrite( &a, sizeof(med), 1, F);
        }

    }else{
        printf(RED"Error: Invalid choice. Try again \n"RESET);
    }
    }while ( choice != 1 && choice != 2 && choice != 3 );}
    else{ 
        while ( i <= pos ){
        fread( &a, sizeof(med), 1, F);
        i++;
        }
        if ( newquan == 0 ){
            fclose(F);
            Delete(F,pos);
        }else{
            a.quantity = newquan;
            fseek(F,-1 * sizeof(med),SEEK_CUR);
            fwrite( &a, sizeof(med), 1, F);
        }

    }
    fclose(F);
}
void UpdatePrice(FILE *F, int pos ,int newprice ){ // to update the price of a med
    F = fopen("meds.bin","rb+");
    med a;
    int i = 0;
    while ( i <= pos ){
        fread( &a, sizeof(med), 1, F);
        i++;
    }
    a.price = newprice;
    fseek(F,-1 * sizeof(med),SEEK_CUR);
    fwrite( &a, sizeof(med), 1, F);
    fclose(F);
}


void addingMedication( date cd , FILE *meds ){ // to add a new medication to the stock
    med M; char a;
    int pos;
    printf("\n-------------------------------\n");
    printf("Enter the name of the medication :\n");
    scanf(" %[^\n]s",M.name);
    printf("Enter the available quantity of your med :\n");
    scanf("%d",&M.quantity);
    printf("Enter its unit price :\n");
    do{
        scanf("%lf",&M.price);
        if ( M.price < 0 )
        printf(RED"Invalid price. Try again .....\n"RESET);
    } while ( M.price < 0 );
    printf("Enter its manufacturing date :\n");
    Date(&M.fxpDate);
    printf("Enter its expiry date :\n");
    Date(&M.expDate);
    if ( DateVerfication( M.fxpDate , M.expDate ) == 0  ){
        do{
            printf(RED"Invalid dates. Please try again....\n"RESET);
            printf("Input its manufacturing date : \n");
            Date(&M.fxpDate);
            printf("Input its expiry date :\n");
            Date(&M.expDate);
        }while( DateVerfication( M.fxpDate , M.expDate ) == 0 );
    }
    if ( DateVerfication ( cd , M.expDate ) == 0 )
    printf(RED"This medicine cannot be added to the stock (expired )\n"RESET);
    else{ 
        printf("Enter the medicine's brand :\n");
        scanf(" %[^\n]s",M.brand);
        warning( M.expDate, cd );
        
        if ( (pos = exist( M.name, M.brand )) != -1 ){ // if the medicine already exist
            printf(YELLOW"This medicine already exist .\n"RESET);
            do{
            printf("Do you want to update its quantity? y/n \n"); // we ask if the user wants to update its quantity ( use the new one )
            scanf(" %c",&a);
            if ( a == 'y' || a == 'Y'){
                UpdateQuantity( meds, pos,M.quantity,0);
                printf(GREEN"Quantity updated!\n"RESET);
            }else if ( a != 'n'  && a != 'N'){
                printf(RED"Wrong input. Try again\n"RESET);
            }
            }while( a != 'y' && a != 'Y' && a != 'n'&& a!= 'N');
            do{
            printf("Do you want to update its price? y/n \n"); // we ask if the user wants to update its price ( use the new one )
            scanf(" %c",&a);
            if ( a == 'y' || a == 'Y'){
                UpdatePrice( meds, pos,M.price);
                printf(GREEN"Price updated!\n"RESET);
            }else if ( a != 'n'  && a!='N'){
                printf(RED"Wrong input. Try again\n"RESET);
            }
            }while( a != 'y' && a != 'Y' && a != 'n'&& a!= 'N');
        }else{
            fseek(meds, 0, SEEK_END);
            fwrite(&M, sizeof(med), 1, meds);
        }
    }
    
}
void Fill( date cd , FILE *meds , int N){ // to fill the file with N meds
    med M; char a;
    int pos, i = 0;
    if ( meds ==  NULL ){
        printf(RED"Error opening file\n"RESET);
        exit(1);
    }
    while( i < N ){
        printf("\n-------------------------------\n");
        printf("Enter the name of the medication :\n");
        scanf(" %[^\n]s",M.name);
        printf("Enter the available quantity of your med :\n");
        scanf("%d",&M.quantity);
        printf("Enter its unit price :\n");
        do{
            scanf("%lf",&M.price);
            if ( M.price < 0 )
            printf(RED"Invalid price. Try again .....\n"RESET);
        } while ( M.price < 0 );
        printf("Enter its manufacturing date :\n");
        Date(&M.fxpDate);
        printf("Enter its expiry date :\n");
        Date(&M.expDate);
        if ( DateVerfication( M.fxpDate , M.expDate ) == 0  ){
            do{
                printf(RED"Invalid dates. Please try again....\n"RESET);
                printf("Input its manufacturing date : \n");
                Date(&M.fxpDate);
                printf("Input its expiry date :\n");
                Date(&M.expDate);
            }while( DateVerfication( M.fxpDate , M.expDate ) == 0 );
        }
        if ( DateVerfication ( cd , M.expDate ) == 0 )
        printf(RED"This medicine cannot be added to the stock (expired )\n"RESET);
        else{ 
            printf("Enter the medicine's brand :\n");
            scanf(" %[^\n]s",M.brand);
            warning( M.expDate, cd );
            fclose(meds);
            if ( exist(  M.name, M.brand ) != -1 ){
            printf(YELLOW"This medicine already exist .\n"RESET);
        }else{
            meds = fopen("meds.bin", "rb+");
            fseek(meds, 0, SEEK_END);
            fwrite(&M, sizeof(med), 1, meds);
        }
        i++;
        }
    }
    fclose(meds);
}
void DisplayMedsList(){ // to display the list of the medicines
    FILE *f = fopen("meds.bin", "rb+");
    med a;
    if ( f == NULL ){
        printf(RED"The stock is empty.\n"RESET);
        return;
    }
    while( fread( &a, sizeof(med), 1, f) == 1 ){
        printf("\n-------------------------------\n");
        printf("\nThe medication's name : "BLUE"\"%s\"\n"RESET,a.name);
        printf("Quantity : "GREEN"%d \n"RESET,a.quantity);
        printf("Price : "GREEN"%.2lf DA\n"RESET,a.price);
        printf("Brand : \"%s\"\n",a.brand);
        printf("Fxp : %d/%d/%d\n",a.fxpDate.day,a.fxpDate.month,a.fxpDate.year);
        printf("Exp : %d/%d/%d\n",a.expDate.day,a.expDate.month,a.expDate.year);
    }
    fclose(f);
    }
    int Search( char *a , int num ){ // to search for a medicine in the stock
        int f = 0;
        FILE *meds = fopen("meds.bin", "rb");
        if ( meds == NULL ){
            printf(RED"The stock is empty.\n"RESET);
            exit(1);
        }
        med M;
        int i = 0,j;
        char temp[num+1];
        while ( fread( &M, sizeof(med), 1, meds) == 1 ){
            j=0;
            while ( j < num ){
                temp[j] = M.name[j];
                j++;
            }
            temp[j]='\0';
            if ( strcmp(temp,a)==0){
                if ( f == 0){
                    
                    printf("\n-------------------------------------- Found Medicines List --------------------------------------\n");
                }
                printf("%d. Med : %s\n",i,M.name);// print the found medicine with their index
                f++;
            }
            i++;
        }
        if( f==0){
            printf(RED"No such medicine found.\n"RESET);
            return 0;
        }
        fclose(meds);
        return 1;

}

int main()
{
    FILE *meds , *medss;
    med m;
    int True = 1 , flag,newquan;
    char a ;
    int N,i,numofchar=0,choice,action,subact;

    date currentdate;
    printf(YELLOW"-------------------------------------------------------- Welcome To The Pharmacy Management System --------------------------------------------------------\n\n"RESET);
    printf("Enter the date : \n");
    Date(&currentdate);
    printf("\n--------------------------------------------------------------------------------------------------\n");
    printf("\nDo you want to retain the existing medication stock file or create a new one?\n");
    printf("1. Keep the existing stock\n");
    printf("2. Create a new one\n");
    printf("\n--------------------------------------------------------------------------------------------------\n");
    do{
    printf(YELLOW"Please choose your action : "RESET);
    scanf(" %c",&a);
    if( a == '2' ){
        meds = fopen("meds.bin","wb+");
        printf("\nEnter the number of medicines in the stock :\n");
        do{
            scanf("%d",&N);
            if ( N < 0 )
            printf(RED"Error! Please enter a positive integer.\n"RESET);
        } while ( N < 0 );
        Fill(currentdate, meds, N);
    }else if ( a == '1' ){
        meds = fopen("meds.bin","rb+");
    }else{
        printf(RED"Error! Please enter a valid choice.\n"RESET);
    }}while( a != '1' && a != '2');
while ( True ){
    printf(RED"\n---------------------------------------------- MENU ----------------------------------------------\n\n"RESET);
    printf("1. Display all the medications in the stock \n");
    printf("2. Add a new medicine to the stock\n");
    printf("3. Search then Modify the quantity of an existing medicine or Remove it from the stock \n");
    printf("4. Exit \n");
    printf(RED"\n--------------------------------------------------------------------------------------------------\n"RESET);
    printf(CYAN"\nPlease choose your action : "RESET);
    do{
    scanf("%d",&action);
    switch(action){
        case  1:
        DisplayMedsList();
        break;
        case 2:
        meds = fopen("meds.bin","rb+");
        addingMedication( currentdate, meds);

        fclose(meds);
        break;
        case 3:
        printf("\nHow many characters are you going to use:\n"); // requesting the number of characters to be used
        scanf("%d", &numofchar);
        char *fc = malloc((numofchar + 1) * sizeof(char));
        if (fc == NULL) {
            printf(RED"Memory allocation failed.\n"RESET);
            break;
        }
        fc[numofchar] = '\0';
        printf("Enter the first %d characters of the name of the medicine that you want to search for :\n", numofchar);
        scanf(" %s", fc);
        if ( Search( fc, numofchar ) != 0 ){
        printf("\n--------------------------------------------------------------------------------------------------\n\n");
        printf("1. Modify the quantity of an existing medicine.\n");
        printf("2. Remove this medicine from the stock.\n");
        printf("3. Return to the main menu.\n");
        printf("\n--------------------------------------------------------------------------------------------------\n\n");
        printf(CYAN"Choose what you want to do:\n"RESET);
        do {
        scanf("%d", &subact);
        if (subact == 1) {
            printf("Enter the number of the medicine which you want to update its quantity:\n");
            scanf("%d", &choice);
            UpdateQuantity(meds, choice,0,1);
            printf(GREEN"Quantity updated!!\n"RESET);
        } else if (subact == 2) {
            printf("Enter the number of the medicine that you want to delete:\n");
            scanf("%d", &choice);
            fclose(meds);
            Delete(meds, choice);
            printf(GREEN"Product deleted!!\n"RESET);
        } else if (subact == 3) {
            //do nothing
        }
        else {
            printf(RED"You only have two options...Try again...\n"RESET);}
        } while (subact != 1 && subact != 2 && subact != 3 );
        free(fc);
        break;
        case 4:
        True = 0;
        medss = fopen("meds.txt","w");
        meds = fopen("meds.bin","rb");
        if (medss == NULL || meds == NULL) {
            printf(RED"Error opening file.\n"RESET);
            exit(1);
        }
        else{
            int j =1;
            while ( fread( &m , sizeof(med), 1, meds)){ // saving whats in the binary file in a textual file
                fprintf(medss,"Medicine number %d :\n",j);
                fprintf(medss,"Name : %s\n",m.name);
                fprintf(medss,"Brand : %s\n",m.brand);
                fprintf(medss,"Price : %.2lf DA\n",m.price);
                fprintf(medss,"Quantity : %d\n",m.quantity);
                fprintf(medss,"Manufacturing date : %d.%d.%d\n",m.fxpDate.day,m.fxpDate.month,m.fxpDate.year);
                fprintf(medss,"Expiry date : %d.%d.%d\n\n\n",m.expDate.day,m.expDate.month,m.expDate.year);
                j++;
            }
        }
        break;
        default:
        printf(RED"\n You only have four options .....\n Try again ......\n"RESET);}
        break;
    }//end switch
    }while ( action < 1 || action > 4);
    }
    return 0;
}