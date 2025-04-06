#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DAYS 7
#define SHIFTS 3

const char *weekdays[DAYS] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const char *shiftNames[SHIFTS] = {"Morning", "Afternoon", "Evening"};
char doctorSchedule[DAYS][SHIFTS][50]; // Doctor names per shift

// ===================
// PATIENT STRUCT
// ===================
typedef struct Patient
{
    int id;
    char name[50];
    int age;
    char diagnosis[20];
    int roomNumber;
    struct tm admitDate;
    struct tm dischargeDate;
    struct Patient *next;
} Patient;

Patient *head = NULL;  // Head of patient list
int nextPatientID = 1; // Keeps track of the next available ID

// ===================
// PATIENT FUNCTIONS
// ===================


//* Adds a new patient to the system
void addPatient()
{
    Patient *newPatient = malloc(sizeof(Patient));
    newPatient->id = nextPatientID++; // Assign the next available ID


    // Get patient details from user
    printf("Enter Patient Name: ");
    fgets(newPatient->name, 50, stdin);
    newPatient->name[strcspn(newPatient->name, "\n")] = 0;

    do
    {
        printf("Enter Age: ");
        scanf("%d", &newPatient->age);
        getchar(); // To clear the newline left by scanf
    }
    while(newPatient->age < 0); // Ensure valid age input

    // Get diagnosis
    printf("Enter Diagnosis: ");
    fgets(newPatient->diagnosis, 20, stdin);
    newPatient->diagnosis[strcspn(newPatient->diagnosis, "\n")] = 0;

    do
    {
        printf("Enter Room Number: "); // todo make room number upper and lower limit (not 0)
        scanf("%d", &newPatient->roomNumber);
        getchar();
    }
    while(newPatient->roomNumber < 0); // Ensure valid room number

    // Get current date and set as admission date
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    newPatient->admitDate = *tm_info; // Set admission date to current date

    // Discharge date is not set here
    memset(&newPatient->dischargeDate, 0, sizeof(struct tm)); // Make discharge date 0 upon admission


    newPatient->next = NULL;

    // Add the new patient to the linked list
    if(!head)
    {
        head = newPatient;
    }
    else
    {
        Patient *temp = head;
        while(temp->next)
            temp = temp->next;
        temp->next = newPatient;
    }

    printf("Patient admitted successfully! Assigned ID: %d\n", newPatient->id);
}

//* Displays all the patients in the system
void viewPatients()
{
    if(!head)
    {
        printf("No patients admitted.\n");
        return;
    }

    Patient *temp = head;
    printf("ID\tName\n");
    while(temp)
    {
        printf("%d\t%s\n",
               temp->id,
               temp->name);
        temp = temp->next;
    }
}

//* Searches for a patient by ID or Name
void searchPatient()
{
    if(!head)
    {
        printf("No patients admitted.\n");
        return;
    }

    int choice, id;
    char name[50];
    Patient *temp = head;

    printf("Search by (1) ID or (2) Name: ");
    scanf("%d", &choice);
    getchar(); // Clear the buffer

    if(choice == 1)
    {
        printf("Enter Patient ID: ");
        scanf("%d", &id);
        getchar();

        while(temp)
        {
            if(temp->id == id)
            {
                printf("ID\tName\tAge\tDiagnosis\tRoom\n");
                printf("%d\t%s\t%d\t%s\t%d\n",
                       temp->id,
                       temp->name,
                       temp->age,
                       temp->diagnosis,
                       temp->roomNumber);
                return;
            }
            temp = temp->next;
        }
    }
    else if(choice == 2)
    {
        printf("Enter Patient Name: ");
        fgets(name, 50, stdin);
        name[strcspn(name, "\n")] = 0;

        while(temp)
        {
            if(strcmp(temp->name, name) == 0)
            {
                printf("ID\tName\tAge\tDiagnosis\tRoom\n");
                printf("%d\t%s\t%d\t%s\t%d\n",
                       temp->id,
                       temp->name,
                       temp->age,
                       temp->diagnosis,
                       temp->roomNumber);
                return;
            }
            temp = temp->next;
        }
    }
    printf("Patient not found.\n");
}

//* Discharges a patient by ID
void dischargePatient()
{
    if(!head) // prevent discharging when no patients exist
    {
        printf("No patients admitted.\n");
        return;
    }

    int id;
    printf("Enter Patient ID to discharge: ");
    scanf("%d", &id);
    getchar();

    Patient *temp = head, *prev = NULL;

    while(temp)
    {
        if(temp->id == id)
        {
            // Set discharge date
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            temp->dischargeDate = *tm_info;

            // Open the discharge file
            FILE *dischargeFile = fopen("discharged.dat", "ab");
            if(!dischargeFile)
            {
                printf("Error opening discharge file!\n");
                return;
            }

            // Create a copy without the next pointer to avoid writing linked list data
            Patient patientCopy = *temp;
            patientCopy.next = NULL;

            if(fwrite(&patientCopy, sizeof(Patient), 1, dischargeFile) != 1)
            {
                printf("Error writing to discharge file!\n");
            }
            fclose(dischargeFile);

            // Remove from active list
            if(prev)
            {
                prev->next = temp->next;
            }
            else
            {
                head = temp->next;
            }

            free(temp);
            printf("Patient discharged successfully.\n");
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Patient not found.\n");
}


// ===================
// DOCTOR SCHEDULE FUNCTIONS
// ===================

//* Assigns a doctor to a shift for a specific day
void assignDoctor()
{
    int day, shift;
    char doctorName[50];

    printf("Select a day:\n");
    for(int i = 0; i < DAYS; i++)
        printf("%d. %s\n", i + 1, weekdays[i]);

    printf("Enter choice: ");
    scanf("%d", &day);
    getchar(); // Clear buffer
    if(day < 1 || day > DAYS)
    {
        printf("Invalid day!\n");
        return;
    }

    printf("Select a shift:\n");
    for(int i = 0; i < SHIFTS; i++)
        printf("%d. %s\n", i + 1, shiftNames[i]);

    printf("Enter choice: ");
    scanf("%d", &shift);
    getchar(); // Clear buffer
    if(shift < 1 || shift > SHIFTS)
    {
        printf("Invalid shift!\n");
        return;
    }

    // Get the doctor's name
    printf("Enter Doctor's Name: ");
    fgets(doctorName, 50, stdin);
    doctorName[strcspn(doctorName, "\n")] = 0;

    // Assign the doctor to the shift
    strcpy(doctorSchedule[day - 1][shift - 1], doctorName);
    printf("Doctor assigned successfully!\n");
}

//* Displays the weekly doctor schedule
void displaySchedule()
{
    printf("\nWeekly Doctor Schedule:\n");
    for(int i = 0; i < DAYS; i++)
    {
        printf("%s:\n", weekdays[i]);
        for(int j = 0; j < SHIFTS; j++)
        {
            printf("  %s: %s\n", shiftNames[j],
                   doctorSchedule[i][j][0] ? doctorSchedule[i][j] : "[Unassigned]");
        }
    }
}


// ===================
// FILE HANDLING FUNCTIONS
// ===================

//* Save patients to a binary file
void savePatients(const char *filename)
{
    //> ERROR Handling, if the file can't be opened then print that it couldn't be opened
    FILE *file = fopen(filename, "wb");
    if(!file)   If fopen fails it would return NULL so this checks if that failed and the file pointer doesn't exist
    {
        printf("Failed to open file for writing.\n");
        return;
    }

    Patient *temp = head;
    while(temp)
    {
        // Create a copy without the next pointer
        Patient patientCopy = *temp;
        patientCopy.next = NULL;

        //> ERROR Handling, checks if Patient struct was written to the binary file 'file'
        if(fwrite(&patientCopy, sizeof(Patient), 1, file) != 1) // should return 1, asked to write 1 item
        {
            printf("Error writing patient data.\n"); // if the file were read only or we ran out of memory, prints
            break;
        }
        temp = temp->next;
    }

    fclose(file);
    printf("Patients saved successfully.\n");
}

void loadPatients(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if(!file)
    {
        printf("No existing patient data found.\n");
        return;
    }

    // Clear existing list
    Patient *temp = head;
    while(temp)
    {
        Patient *next = temp->next;
        free(temp);
        temp = next;
    }
    head = NULL;
    nextPatientID = 1;

    // Load new data
    Patient patientBuffer;
    Patient *last = NULL;

    while(fread(&patientBuffer, sizeof(Patient), 1, file) == 1)
    {
        Patient *newPatient = malloc(sizeof(Patient));
        *newPatient = patientBuffer;
        newPatient->next = NULL;

        if(!head)
        {
            head = newPatient;
        }
        else
        {
            last->next = newPatient;
        }
        last = newPatient;

        // Update next available ID
        if(newPatient->id >= nextPatientID)
        {
            nextPatientID = newPatient->id + 1;
        }
    }

    fclose(file);
    printf("Patients loaded successfully.\n");
}


void saveDoctorSchedule(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if(!file)
    {
        printf("Failed to save doctor schedule.\n");
        return;
    }

    fwrite(doctorSchedule, sizeof(doctorSchedule), 1, file);
    fclose(file);
    printf("Doctor schedule saved successfully.\n");
}

void loadDoctorSchedule(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if(!file)
    {
        printf("No saved doctor schedule found.\n");
        return;
    }

    fread(doctorSchedule, sizeof(doctorSchedule), 1, file);
    fclose(file);
    printf("Doctor schedule loaded successfully.\n");
}

// ===================
// REPORTS
// ===================

void generateAdmissionReport(int reportType)
{
    int count = 0;

    time_t now = time(NULL);
    struct tm *currentDate = localtime(&now);

    Patient *temp = head;
    while(temp)
    {
        struct tm admit = temp->admitDate;

        // Calculate difference in days between admit date and today
        int yearDiff = currentDate->tm_year - admit.tm_year;
        int monthDiff = currentDate->tm_mon - admit.tm_mon;
        int dayDiff = currentDate->tm_mday - admit.tm_mday;

        // Convert everything into a rough day difference
        int daysAgo = yearDiff * 365 + monthDiff * 30 + dayDiff;

        if(reportType == 1 && yearDiff == 0 && monthDiff == 0 && dayDiff == 0)
        {
            count++; // Admitted today
        }
        else if(reportType == 2 && daysAgo >= 0 && daysAgo <= 6)
        {
            count++; // Within last 7 days
        }
        else if(reportType == 3 && daysAgo >= 0 && daysAgo <= 29)
        {
            count++; // Within last 30 days
        }

        temp = temp->next;
    }

    if(reportType == 1)
    {
        printf("%d patients admitted today:\n", count);
    }
    else if(reportType == 2)
    {
        printf("%d patients admitted in the last week:\n", count);
    }
    else if(reportType == 3)
    {
        printf("%d patients admitted in the last month:\n", count);
    }
}

void generateDischargeReport(struct tm dischargeDate)
{
    FILE *file = fopen("discharged.dat", "rb");
    if(!file)
    {
        printf("No discharged patients found.\n");
        return;
    }

    printf("Discharged patients on %d-%d-%d:\n",
           dischargeDate.tm_mday,
           dischargeDate.tm_mon + 1,
           dischargeDate.tm_year + 1900);

    Patient patient;
    int count = 0;

    printf("ID\tName\n");
    while(fread(&patient, sizeof(Patient), 1, file) == 1)
    {
        // Compare discharge dates
        if(patient.dischargeDate.tm_mday == dischargeDate.tm_mday &&
           patient.dischargeDate.tm_mon == dischargeDate.tm_mon &&
           patient.dischargeDate.tm_year == dischargeDate.tm_year)
        {
            printf("%d\t%s\n",
                   patient.id,
                   patient.name);
            count++;
        }
    }

    if(count == 0)
    {
        printf("No patients discharged on this date.\n");
    }

    fclose(file);
}


// ===================
// MENU SYSTEMS
// ===================

//* Main menu or specific report menu
void reportMenu()
{
    int choice;
    do
    {
        printf("\nReport Menu:\n");
        printf("1. Generate Admission Report (Daily)\n");
        printf("2. Generate Admission Report (Weekly)\n");
        printf("3. Generate Admission Report (Monthly)\n");
        printf("4. Generate Discharge Report\n");
        printf("5. Generate Doctor Utilization Report\n");
        printf("6. Generate Room Usage Report\n");
        printf("7. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar(); // Clear buffer

        switch(choice)
        {
            case 1: generateAdmissionReport(1);
                break;
            case 2: generateAdmissionReport(2);
                break;
            case 3: generateAdmissionReport(3);
                break;
            case 4:
            {
                struct tm dischargeDate;
                printf("Enter discharge date (day month year): ");
                scanf("%d %d %d", &dischargeDate.tm_mday, &dischargeDate.tm_mon, &dischargeDate.tm_year);
                dischargeDate.tm_mon--;        // Adjust for tm struct (0-11)
                dischargeDate.tm_year -= 1900; // Adjust for tm struct
                generateDischargeReport(dischargeDate);
                break;
            }
            case 5: //TODO generateDoctorUtilizationReport();
                break;
            case 6: //TODO generateRoomUsageReport();
                break;
            case 7: return;
            default: printf("Invalid choice!\n");
        }
    }
    while(choice != 7);
}

//* Main menu for hospital management
void menu()
{
    int choice;
    do
    {
        printf("\nHospital Management System\n");
        printf("1. Add Patient\n");
        printf("2. View Patients\n");
        printf("3. Search Patient\n");
        printf("4. Discharge Patient\n");
        printf("5. Manage Doctor Schedule\n");
        printf("6. Open Report Menu\n");
        printf("7. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar(); // Clear buffer

        switch(choice)
        {
            case 1: addPatient();
                break;
            case 2: viewPatients();
                break;
            case 3: searchPatient();
                break;
            case 4: dischargePatient();
                break;
            case 5:
            {
                int sub;
                do
                {
                    printf("\nDoctor Schedule Menu\n");
                    printf("1. Assign Doctor\n");
                    printf("2. Display Schedule\n");
                    printf("3. Back\n");
                    printf("Enter choice: ");
                    scanf("%d", &sub);
                    getchar(); // Clear buffer
                    if(sub == 1) assignDoctor();
                    else if(sub == 2) displaySchedule();
                }
                while(sub != 3);
            }
            break;
            case 6:
                reportMenu();
                break;
            case 7: printf("Exiting...\n");
                break;
            default: printf("Invalid choice!\n");
        }
    }
    while(choice != 7);
}

// ===================
// MAIN FUNCTION
// ===================

int main()
{
    //load save files
    loadPatients("patients.dat");
    loadDoctorSchedule("doctorSchedule.dat");

    menu();

    //save files
    savePatients("patients.dat");
    saveDoctorSchedule("doctorSchedule.dat");

    return 0;
}
