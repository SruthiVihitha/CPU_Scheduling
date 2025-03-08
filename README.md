**README - Process Scheduler using GTK**

### Overview
This project implements a **Process Scheduling Simulation** using **GTK+** for the graphical user interface. It provides scheduling for **Shortest Job Next (SJN) Non-Preemptive** and **Priority Scheduling Non-Preemptive** algorithms.

### Features
- **Graphical User Interface (GUI)** using GTK+.
- **Process Addition:** Users can input **Process ID (PID), Arrival Time (AT), Burst Time (BT)**, and **Priority (for priority scheduling).**
- **Simulation Execution:** Runs the scheduling algorithm and updates the table.
- **Gantt Chart Visualization:** Uses **Cairo graphics** to display the scheduling order.
- **Table View:** Displays **PID, AT, BT, CT (Completion Time), TAT (Turnaround Time), WT (Waiting Time).**
- **Average TAT & WT Calculation.**

### Prerequisites
- **GTK+ 3** installed on the system.
- **GCC compiler** for compilation.

### Installation & Compilation
Ensure you have GTK installed. If not, install it using:
```sh
sudo apt-get install libgtk-3-dev # for Ubuntu/Debian
yum install gtk3-devel # for Fedora
```
Compile the program using:
```sh
gcc -o scheduler scheduler.c `pkg-config --cflags --libs gtk+-3.0`
```
Run the executable:
```sh
./scheduler
```
![image](https://github.com/user-attachments/assets/75c46d9d-abec-4dd1-a5cc-ac1914f2d45a)


### Usage
1. Enter **PID, Arrival Time, Burst Time** (and **Priority** for Priority Scheduling).
2. Click **Add Process** to add it to the list.
3. Click **Simulate** to run the scheduling algorithm.
4. Observe **Process Execution Order** in the **Gantt Chart.**
5. View the computed **CT, TAT, WT, and Average Times.**

### Visual Representation
#### **Process Table Format**
| PID | Arrival Time (AT) | Burst Time (BT) | Completion Time (CT) | Turnaround Time (TAT) | Waiting Time (WT) |
|-----|------------------|----------------|------------------|------------------|------------------|
| P1  | 0               | 5              | 5                | 5                | 0                |
| P2  | 1               | 3              | 8                | 7                | 4                |
| P3  | 2               | 8              | 16               | 14               | 6                |

#### **Gantt Chart Representation**

| ▓▓ | ▓▓ | ▓▓ | ▓▓ | ▓▓ |  

### **Outputs:**
### **SJF SCHEDULING:**
![image](https://github.com/user-attachments/assets/a4c70751-b07c-4e71-973c-6f1b1491dbbf)
### **PRIORITY SCHEDULING:**
![image](https://github.com/user-attachments/assets/c3c0ef9e-deae-4bf1-8042-9cced2dc4892)


This project serves as an interactive way to understand and visualize **Process Scheduling Algorithms** with a **GUI-based simulation.**

