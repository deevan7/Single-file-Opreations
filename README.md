**README_Single File Opreations**
Deevankumar


**File Handling:**
    - Creates files and directories.
    - Appends text to text files and odd numbers to binary files with 50 bytes after the number user enters and stops at max of 199 if it cross more than 50.
    - Writes the first 50 bytes of a file to standard output.
    - Deletes files and empty directories.
    - Renames files and directories.
- **Error Handling:**
    - Returns system-defined error codes for potential issues.

**Available Operations**

**create:** Creates a file.
**createdir:** Creates a directory.
**delete:** Deletes a file or empty directory.
**rename:** Renames a file or directory.
**appendtext:** appendtext appends text to the given file.
**appendbinary:** appendbinary appends the oddnumbers if inputs numbver between 50-200.
**head -c 50:** prints first 50 bytes of the file.

**Examples**
compling the code with the given format
gcc my_fm.c -o myfm
 
 - Create a file named "testfile.txt":
`./my_fm create testfile.txt`


- Create a directory named "newdir":
`./my_fm createdir newdir`


- Delete the file "testfile.txt":
`./my_fm delete testfile.txt`


- Rename the file "oldname.txt" to "newname.txt":
`./my_fm rename newdir latestdir //changes directory name form newdir to latestdir`


- Appends the test given in "quotes" and numbers to the file:
`./my_fm appendtext filename "the text we want to add"`


- Appends odd numbers between 50-200(51-199 then from 01 - 50 bytes of the given number by max:
`./my_fm appendbinary objectfile (oddnumber between 50 to 200)`
then it prints most 50 bytes of that or it will start from 0103.....(50 bytes)


- prints the 50 bytes of the given file:
`head -c 50 filename(prints the first fifty bytes of the given filename(anyfile))`
**Important Notes**

- The program does not print error messages; it only returns error codes.
- It does not handle operations on directories other than creation and deletion.


In the folder we already have the file known as sample(object file) which was tested before and printed odd numbers  51 to 99(50 bytes) when we run the code ./my_fm appendbinary sample 51

appended text in the file named deevan (Single File Opreations Assignment) in the text file
