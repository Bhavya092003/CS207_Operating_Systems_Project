
# Part F — File-System and OS Case-Study Analysis

## 1. Objective

The objective of this experiment is to study Linux file-system organization, file paths, permissions, ownership and metadata. It also compares Linux and Windows operating systems in terms of process management, file-system organization, user permissions and command-line tools.

The practical observations were collected using Ubuntu 24.04 on WSL and Windows PowerShell 5.1.

## 2. Sample Directory Tree

A sample organizational directory structure was created using Ubuntu terminal commands.

### Commands Used

```bash
mkdir -p part_f_demo/department/engineering/projects/os
mkdir -p part_f_demo/department/hr/recruitment
mkdir -p part_f_demo/shared
```

Six files were created using the `touch` command.

```bash
touch part_f_demo/department/engineering/engineers.txt
touch part_f_demo/department/engineering/projects/project_list.txt
touch part_f_demo/department/engineering/projects/os/kernel_notes.txt
touch part_f_demo/department/hr/employees.txt
touch part_f_demo/department/hr/recruitment/openings.txt
touch part_f_demo/shared/announcement.txt
```

### Resulting Structure

```text
part_f_demo/
├── department/
│   ├── engineering/
│   │   ├── engineers.txt
│   │   └── projects/
│   │       ├── project_list.txt
│   │       └── os/
│   │           └── kernel_notes.txt
│   └── hr/
│       ├── employees.txt
│       └── recruitment/
│           └── openings.txt
└── shared/
    └── announcement.txt
```

The directory structure was verified using:

```bash
find part_f_demo -print
```

## 3. Absolute and Relative Paths

An absolute path specifies the complete location of a file starting from the root directory.

A relative path specifies a file location with respect to the current working directory.

### Commands Used

```bash
pwd
```

Output:

```text
/mnt/d/CS207_Operating_Systems_Project
```

To display the absolute path of a file:

```bash
realpath part_f_demo/department/engineering/projects/os/kernel_notes.txt
```

Output:

```text
/mnt/d/CS207_Operating_Systems_Project/part_f_demo/department/engineering/projects/os/kernel_notes.txt
```

Relative path demonstration:

```bash
cd part_f_demo/department/engineering
ls projects/os/kernel_notes.txt
```

Output:

```text
projects/os/kernel_notes.txt
```

## 4. File Permissions

Linux uses three permission categories:

- User (u): File owner.
- Group (g): Users belonging to the associated group.
- Others (o): All other users.

The basic permissions are:

| Permission | Meaning | Numeric value |
|---|---|---:|
| r | Read | 4 |
| w | Write | 2 |
| x | Execute | 1 |

The following commands were executed:

```bash
chmod 644 part_f_demo/department/engineering/engineers.txt
chmod 600 part_f_demo/department/hr/employees.txt
```

However, the displayed permissions remained `rwxrwxrwx` (777).

This happened because the project is stored under `/mnt/d`, a Windows-mounted drive in WSL. Its displayed permission behavior depends on the WSL DrvFS mount configuration.

The permissions were inspected using:

```bash
ls -lR part_f_demo
```

Example observed output:

```text
-rwxrwxrwx 1 bhavya bhavya 0 ... engineers.txt
```

Thus, the experiment also demonstrates that Linux permission behavior can differ when accessing Windows-mounted filesystems through WSL.

## 5. Ownership and File Metadata

The following commands were used:

```bash
whoami
id
ls -lR part_f_demo
stat part_f_demo/department/engineering/engineers.txt
```

Observed user and group information:

```text
uid=1000(bhavya) gid=1000(bhavya)
```

The `ls -l` output displayed the owner and group of the files.

The `stat` command displayed detailed metadata:

| Attribute | Observed value |
|---|---|
| File type | Regular empty file |
| File size | 0 bytes |
| Inode | 281474976711075 |
| Hard links | 1 |
| Permissions | 0777 |
| UID | 1000 |
| GID | 1000 |
| Access time | Displayed |
| Modification time | Displayed |
| Change time | Displayed |

## 6. Linux and Windows Comparison

### 6.1 Process Management

Linux commands:

```bash
ps aux | head -n 10
pstree -p | head -n 20
ps -ef --forest | head -n 20
top -b -n 1 | head -n 12
```

Windows PowerShell commands:

```powershell
Get-Process | Select-Object -First 10
Get-Process -Id $PID
Get-Process | Sort-Object CPU -Descending |
    Select-Object -First 10 ProcessName, Id, CPU
Get-CimInstance Win32_Process |
    Select-Object -First 10 ProcessId, ParentProcessId, Name
```

Observations:

- Linux uses `ps` to list processes and `top` to monitor resource utilization.
- `pstree` displays process parent-child relationships.
- Windows uses `Get-Process` to list and inspect processes.
- `Get-CimInstance Win32_Process` provides process IDs and parent process IDs.
- Both operating systems identify processes using PIDs and maintain process relationships.

### 6.2 File-System Organization

Linux commands:

```bash
ls /
df -h
find part_f_demo -maxdepth 5 -print
```

Windows PowerShell commands:

```powershell
Get-PSDrive -PSProvider FileSystem
Get-Location
Get-ChildItem C:\Users\Bhavya | Select-Object -First 10
```

Observations:

- Linux organizes its directory hierarchy under a single root directory `/`.
- Windows commonly organizes storage using drive letters such as `C:\`, `D:\` and `E:\`.
- Linux mounts additional file systems at directories within its hierarchy.
- Windows provides drive-based access to storage volumes.

### 6.3 User Permissions and Protection

Linux commands:

```bash
whoami
id
ls -l part_f_demo
stat part_f_demo/department/engineering/engineers.txt
```

Windows PowerShell commands:

```powershell
Get-Acl C:\Users\Bhavya\Documents | Format-List
whoami
whoami /groups
```

Observations:

- Linux uses UID and GID to identify users and groups.
- Linux file permissions are represented using read, write and execute bits for owner, group and others.
- Windows uses Security Identifiers (SIDs) to identify users and groups.
- Windows Access Control Lists (ACLs) specify permissions for individual security principals.
- The Windows Documents folder inspection showed Full Control permissions for the current user, Administrators and SYSTEM.

### 6.4 Command-Line Tools

| Operation | Linux | Windows PowerShell |
|---|---|---|
| List files | `ls` | `Get-ChildItem` |
| Current directory | `pwd` | `Get-Location` |
| Change directory | `cd` | `Set-Location` |
| Display processes | `ps` | `Get-Process` |
| Process hierarchy/details | `pstree`, `ps --forest` | `Get-CimInstance` |
| File metadata | `stat` | `Get-Item` |
| File permissions | `chmod`, `ls -l` | `Get-Acl` |
| Disk usage | `df -h` | `Get-PSDrive` |

Linux commonly uses standalone command-line utilities, whereas PowerShell provides cmdlets that return structured objects which can be passed through pipelines.

## 7. Overall Comparison

| Area | Linux | Windows |
|---|---|---|
| Process management | Utilities such as `ps`, `top` and `pstree` | PowerShell, Task Manager and CIM |
| File-system structure | Single root hierarchy | Drive-letter-based organization |
| User identification | UID and GID | SIDs |
| Permission model | Owner/group/others permission bits | ACL-based access control |
| Command-line interface | Bash and other shells | PowerShell and Command Prompt |

Both operating systems provide process management, hierarchical file organization and access protection, but their implementation and administration tools differ.

## 8. Conclusion

The experiment successfully demonstrated the creation and inspection of a nested Linux directory structure, absolute and relative paths, file permissions, ownership and file metadata.

The comparison of Linux and Windows showed differences in process-management utilities, file-system organization, permission models and command-line tools.

Linux uses a unified root directory and UID/GID-based permission mechanisms, whereas Windows commonly uses drive letters and ACL-based access control.

The practical commands and their outputs provided evidence of these differences.

**Environment note:** The Linux experiments were performed using Ubuntu 24.04 under WSL 2, while the Windows observations were collected using Windows PowerShell 5.1. WSL is not a VirtualBox virtual machine.
