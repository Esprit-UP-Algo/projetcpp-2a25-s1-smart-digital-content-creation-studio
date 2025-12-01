# QR Code Feature Update - Urgent Projects Deadline Warnings

## Summary
Modified the QR code generation feature to display **simple deadline warning messages** for projects with deadlines within the next 7 days. The QR code now shows concise alerts about when each project's deadline will expire, rather than showing all project details.

## Changes Made

### 1. **projet.h** - Added New Method Declaration
- Added `afficherProjetsUrgents()` method declaration to retrieve urgent projects

### 2. **projet.cpp** - Implemented New Method
- Implemented `afficherProjetsUrgents()` method
- This method queries the database for all projects where `DEADLINE <= SYSDATE + 7`
- Returns a `QSqlQueryModel*` with projects ordered by deadline (ascending)
- Returns `nullptr` if query fails or no projects found

### 3. **gemploye.cpp** - Modified QR Code Generation

#### Modified `generateProjectQRCode(int row)`
**Previous behavior:**
- Generated QR code for a single selected project from the table
- Showed: Code, Title, Budget, and Deadline of one project

**New behavior:**
- Ignores the `row` parameter (marked as `Q_UNUSED`)
- Calls `afficherProjetsUrgents()` to get all projects with deadlines ≤ 7 days
- Calculates days remaining for each project
- Generates a QR code containing **simple warning messages**:
  - Header: "⚠️ URGENT PROJECT DEADLINES ⚠️"
  - For each project, one of these messages:
    - **Expired**: "❌ Project 'ProjectName' deadline EXPIRED X day(s) ago!"
    - **Today**: "🔴 Project 'ProjectName' deadline expires TODAY!"
    - **1 day**: "⏰ Project 'ProjectName' deadline expires in 1 day!"
    - **Multiple days**: "⏰ Project 'ProjectName' deadline expires in X days"
  - Footer: Total count of urgent projects
- Shows a larger dialog (400x500)
- Displays appropriate message if no urgent projects found

**What's NOT shown anymore:**
- ❌ Project Code
- ❌ Budget
- ❌ Raw deadline date

**What IS shown now:**
- ✅ Project Title
- ✅ Days until deadline expires (or how many days ago it expired)
- ✅ Clear visual indicators (emojis) for urgency level

#### Modified `on_generateQRCode_clicked()`
**Previous behavior:**
- Required user to select a project from table first
- Showed warning if no project selected

**New behavior:**
- Immediately generates QR code for urgent projects
- No selection required
- Simply calls `generateProjectQRCode(0)` with unused parameter

## Database Query
The new method uses this SQL query:
```sql
SELECT CODE, TITRE, BUDGET, TO_CHAR(DEADLINE, 'YYYY-MM-DD') AS DEADLINE 
FROM PROJET 
WHERE DEADLINE <= SYSDATE + 7 
ORDER BY DEADLINE ASC
```

## Example QR Code Content
```
⚠️ URGENT PROJECT DEADLINES ⚠️

🔴 Project 'Website Redesign' deadline expires TODAY!

⏰ Project 'Mobile App' deadline expires in 2 days

⏰ Project 'Database Migration' deadline expires in 5 days

Total: 3 urgent project(s)
```

## Usage
1. Click the "Generate QR Code" button in the project management interface
2. A dialog will appear showing:
   - "⚠️ Deadline Warnings" title
   - Number of urgent projects (deadline ≤ 7 days)
   - QR code containing simple expiration warnings
   - Instruction: "Scan to see which projects are expiring soon"
3. Scan the QR code with a mobile device to quickly see which projects need attention

## Benefits
- **Focused Information**: Shows only the most important info - what's expiring and when
- **Easy to Read**: Simple messages instead of technical details
- **Visual Urgency**: Emoji indicators help quickly identify critical projects
- **No Selection Required**: Easier to use, no need to select a project first
- **Better Planning**: Immediately see how many days you have left for each project
- **Clean Format**: No clutter from codes and budgets - just the essential deadline warnings

