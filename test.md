To address the **Reporting and Analytics** requirement, we can add functions to generate various reports, format them neatly, and save them to text files. These reports will help in decision-making and track various hospital metrics like patient admissions, discharges, doctor utilization, and room usage.

Here’s how we can implement the required functionalities:

### Step 1: Define Report Functions

#### 1.1 **Generate a Report of Total Number of Patients Admitted (Daily/Weekly/Monthly)**

We can add a function that takes a date range (e.g., daily, weekly, or monthly) and counts the number of patients admitted during that period. For simplicity, we can assume all patients have a general admission date associated with them. For this example, we’ll add a date field to the `Patient` structure.

**Code**:

```c
// Add a field for the admission date in the Patient structure
typedef struct Patient {
    int id;
    char name[50];
    int age;
    char diagnosis[20];
    int roomNumber;
    struct tm admissionDate;  // Admission date (using tm structure from time.h)
    struct Patient *next;
} Patient;

// Function to generate a report for total patients admitted in a specific range (daily/weekly/monthly)
void generateAdmissionReport(int reportType) {
    int count = 0;
    time_t currentTime = time(NULL);
    struct tm *currentDate = localtime(&currentTime);

    Patient *temp = head;
    while (temp) {
        // Calculate if the patient’s admission is within the specified range
        // Report by day, week, or month
        if (reportType == 1) {  // Daily report
            if (temp->admissionDate.tm_yday == currentDate->tm_yday && temp->admissionDate.tm_year == currentDate->tm_year)
                count++;
        } else if (reportType == 2) {  // Weekly report
            // Check if within the same week
            if (temp->admissionDate.tm_yday / 7 == currentDate->tm_yday / 7)
                count++;
        } else if (reportType == 3) {  // Monthly report
            if (temp->admissionDate.tm_mon == currentDate->tm_mon && temp->admissionDate.tm_year == currentDate->tm_year)
                count++;
        }
        temp = temp->next;
    }

    printf("Total patients admitted: %d\n", count);
}
```

#### 1.2 **Generate a List of Discharged Patients on a Specific Day**

We will create a function to track discharged patients. For this, we could add a discharge date field to the `Patient` structure and then filter patients based on their discharge date.

**Code**:

```c
// Add a field for the discharge date in the Patient structure
typedef struct Patient {
    int id;
    char name[50];
    int age;
    char diagnosis[20];
    int roomNumber;
    struct tm admissionDate;  // Admission date
    struct tm dischargeDate;  // Discharge date
    struct Patient *next;
} Patient;

// Function to generate a list of discharged patients on a specific day
void generateDischargeReport(struct tm dischargeDate) {
    int count = 0;
    Patient *temp = head;

    printf("Discharged patients on %d-%d-%d:\n", dischargeDate.tm_mday, dischargeDate.tm_mon + 1, dischargeDate.tm_year + 1900);
    while (temp) {
        if (temp->dischargeDate.tm_mday == dischargeDate.tm_mday &&
            temp->dischargeDate.tm_mon == dischargeDate.tm_mon &&
            temp->dischargeDate.tm_year == dischargeDate.tm_year) {
            printf("Patient ID: %d, Name: %s\n", temp->id, temp->name);
            count++;
        }
        temp = temp->next;
    }

    if (count == 0) {
        printf("No patients discharged on this date.\n");
    }
}
```

#### 1.3 **Generate Doctor Utilization Report (Weekly)**

To track doctor shifts, we’ll need to count how many shifts each doctor has covered during a week. We can create a function that tallies the shifts and displays the utilization report.

**Code**:

```c
// Function to generate a doctor utilization report (weekly)
void generateDoctorUtilizationReport() {
    int shiftCount[7][3] = {0};  // Initialize shift count for each day and shift (week days x 3 shifts)

    // Count doctor shifts from the doctorSchedule array
    for (int i = 0; i < DAYS; i++) {
        for (int j = 0; j < SHIFTS; j++) {
            if (doctorSchedule[i][j][0] != '\0') {  // If doctor is assigned
                shiftCount[i][j]++;
            }
        }
    }

    printf("Doctor Utilization Report (Weekly):\n");
    for (int i = 0; i < DAYS; i++) {
        printf("%s:\n", weekdays[i]);
        for (int j = 0; j < SHIFTS; j++) {
            printf("  %s: %d shifts\n", shiftNames[j], shiftCount[i][j]);
        }
    }
}
```

#### 1.4 **Generate Room Usage Report**

We will count how many patients are assigned to each room and identify which rooms are underutilized or overutilized.

**Code**:

```c
// Function to generate a room usage report
void generateRoomUsageReport() {
    int roomCount[200] = {0};  // Assume a maximum of 200 rooms
    Patient *temp = head;

    // Count the number of patients in each room
    while (temp) {
        roomCount[temp->roomNumber]++;
        temp = temp->next;
    }

    printf("Room Usage Report:\n");
    for (int i = 0; i < 200; i++) {
        if (roomCount[i] > 0) {
            printf("Room %d: %d patients\n", i, roomCount[i]);
        }
    }
}
```

### Step 2: Save Reports to Text Files

We will add a function to save the generated reports to text files for later reference.

**Code**:

```c
// Function to save a report to a text file
void saveReportToFile(const char *filename, const char *report) {
    FILE *file = fopen(filename, "w");  // Open file in write mode
    if (!file) {
        printf("Failed to save report.\n");
        return;
    }

    fprintf(file, "%s", report);  // Write the report to the file
    fclose(file);  // Close the file
    printf("Report saved to %s.\n", filename);
}
```

### Step 3: Putting It All Together

Now we can integrate these functions into the hospital management system.

**Code Example for Integrating Reports**:

```c
// Main menu or specific report menu
void reportMenu() {
    int choice;
    do {
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

        switch (choice) {
            case 1: generateAdmissionReport(1); break;
            case 2: generateAdmissionReport(2); break;
            case 3: generateAdmissionReport(3); break;
            case 4: {
                struct tm dischargeDate;
                printf("Enter discharge date (day month year): ");
                scanf("%d %d %d", &dischargeDate.tm_mday, &dischargeDate.tm_mon, &dischargeDate.tm_year);
                dischargeDate.tm_mon--;  // Adjust for tm struct (0-11)
                dischargeDate.tm_year -= 1900;  // Adjust for tm struct
                generateDischargeReport(dischargeDate);
                break;
            }
            case 5: generateDoctorUtilizationReport(); break;
            case 6: generateRoomUsageReport(); break;
            case 7: return;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 7);
}
```

### Step 4: Testing the Reports

- **Test the Report Generation**: Run the program, generate reports, and confirm they’re displayed correctly.
- **Test Saving Reports**: Call the `saveReportToFile()` function after generating each report to save it to a text file.

By implementing these reporting and analytics functions, your hospital management system will provide valuable insights into admissions, discharges, doctor utilization, and room usage, all of which can be saved to text files for later reference.