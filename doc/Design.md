# Function Design
## Control System
- receive input from command line or GUI
- check whether the input is valid and whether the user has permission
- interact with value and provide output
## Account System
- maintain account information
- provide interface to log in, register, change password
- maintain current user information, including permission level
## Book System
- maintain book information
- provide interface to search, buy, stock up, enter information, modify information
## Log System
- maintain log information, including time and income/outcome
- provide interface to check log by time
# Data Design
- string[i] means a string with length at most i
- \* means unique
## Account
- *UserID: string[30]
- Password: string[30]
- Username: string[30]
- Privilege: int
## Book
- *UUID: unsigned int
- *ISBN_TYPE: string[20]
- BookName: string[60]
- Author: string[60]
- Keyword: string[60]
    - multiple keywords are separated by '|'
- Stock: unsigned int
- Price: unsigned float
## Log
- *Tick: unsigned int
### Income & Outcome Log
- UUID
- Stock
- Price
### System Log
- User: string[30]
- Command: string[200]
# Interaction Design
- When failed, simply print "Invalid\n", including syntax error or permission level not enough
- [i] means the command requires permission level i
- use regex to express input format. see Data Design for parameter format
## Basic
### Quit[0]
- Input: quit
- Usage: Exit the program
### Exit[0]
- Input: exit
- Usage: Exit the program
## Account
### Switch User[0]
- Input: su [UserID] ([Password])?
  - password can be omitted if current user has higher permission level
- Usage: Push account [UserID] to the stack
- Output: 
    - Fail: 
        - User not exist
        - Wrong password
        - Password not provided and current user has lower permission level
### Logout[1]
- Input: logout
- Usage: Pop the current user from the stack
- Output: 
    - Fail: 
      - No user logged in
### Register[0]
- Input: register [UserID] [Password] [Username]
- Usage: Register a new user with permission level 1
- Output: 
    - Fail: 
        - UserID already exist
### Change Password[1]
- Input: passwd [UserID] ([CurrentPassword])? [NewPassword]
    - CurrentPassword can be omitted if current user has higher permission level
- Usage: Change the password of [UserID] to [NewPassword]
- Output: 
    - Fail: 
        - User not exist
        - Wrong password
        - Password not provided and current user has lower permission level
### Create User[3]
- Input: useradd [UserID] [Password] [Privilege] [Username]
- Usage: Create a new user with information provided
- Output: 
    - Fail: 
        - UserID already exist
        - Privilege level bigger than current user
### Delete User[7]
- Input: delete [UserID]
- Usage: Delete the user with [UserID]
- Output: 
    - Fail: 
        - User not exist
        - User logged in
## Book
### Search[1]
- Input: show (-ISBN_TYPE=[ISBN_TYPE] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]")+
- Usage: Search for books with given information. Support multiple search conditions and multiple keywords
- Output: 
    - Success: Print [ISBN_TYPE]\t[BookName]\t[Author]\t[Keyword]\t[Price]\t[stock]\n for each book with ascending order of ISBN_TYPE
### Buy[1]
- Input: buy [ISBN_TYPE] [Stock]
- Usage: Buy [Stock] books with [ISBN_TYPE]
- Output: 
  - Success: Print total price, i.e., price * [Stock]
  - Fail: 
      - Book not exist
      - Not enough stock
### Add[3]
- Input: add [ISBN_TYPE] [Bookname] [Author] [Keyword] [Stock] [Price]
- Usage: Add books with given information for the first time
- Output: 
  - Fail: 
      - Book already exist
### Modify[3]
- Input: modify [ISBN_TYPE] (-ISBN_TYPE=[ISBN_TYPE] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]" | -price=[Price])+
- Usage: Modify books with given information. Overwrite all keywords if provided
- Output: 
  - Fail: 
      - Book not exist
      - Multiple arguments for one field
      - ISBN_TYPE already exists
### Import[3]
- Input: import [ISBN_TYPE] [Stock] [Price]
- Usage: Import [Stock] books with total cost [Price]
- Output: 
  - Fail: 
      - Book not exist
## Log
### Check Income[7]
- Input: income [Tick1] [Tick2]
- Usage: Check income between [Tick1] and [Tick2]
- Output: 
  - Success: Print [ISBN_TYPE]\t[Price]\t[stock]\n for each income log
### Check Income[7]
- Input: outcome [Tick1] [Tick2]
- Usage: Check outcome between [Tick1] and [Tick2]
- Output:
    - Success: Print [ISBN_TYPE]\t[Price]\t[stock]\n for each outcome log
### Check Earning[7]
- Input: earning [Tick1] [Tick2]
- Usage: Check earning between [Tick1] and [Tick2]
- Output:
    - Success: Print [Income]\t[Outcome]\t[Earning]\n
      - Income: sum of price * stock for each income log
      - Outcome: sum of price * stock for each outcome log
      - Earning: Income - Outcome
### Check Staff Log[7]
- Input: staff [UserID] [Tick1] [Tick2]
- Usage: Check system log of [UserID] between [Tick1] and [Tick2]
- Output:
    - Success: Print [Command]\n for each system log 
    - Fail: User not exist
### Check System Log[7]
- Input: system [Tick1] [Tick2]
- Usage: Check system log between [Tick1] and [Tick2]
- Output:
    - Success: Print [User]\t[Command]\n for each system log
# Class & Struct Design
- put all methods(as above) and classes(as follows, in seperated files) in a namespace
- Am I allowed to use sth. like clipp?
## Control
- Slice
- Command
## Account
- AccountStack
- Account
## Book
- Book
## Log
- EarningLog
- SystemLog
## Database
- FileStorage
- PersistentSet
- PersistentMap
- DataTypes
  - FixedString
  - ISBN_TYPE