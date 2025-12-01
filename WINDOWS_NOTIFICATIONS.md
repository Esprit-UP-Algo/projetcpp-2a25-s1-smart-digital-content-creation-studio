# Windows System Notifications for Project Deadlines

## Summary
Added native Windows system notifications (toast notifications) that appear in the Windows notification center to alert users about urgent project deadlines. These notifications appear automatically instead of relying on the in-app notification panel.

## What Changed

### 1. **Header File (gemploye.h)**
**Added:**
- `#include <QSystemTrayIcon>` - For Windows notification support
- `void afficherNotificationsWindows()` - New method declaration
- `QSystemTrayIcon *systemTrayIcon` - Member variable for system tray

### 2. **Implementation File (gemploye.cpp)**

#### Constructor (Gemploye::Gemploye)
**Added initialization:**
```cpp
// Initialize System Tray Icon for Windows notifications
systemTrayIcon = new QSystemTrayIcon(this);
systemTrayIcon->setIcon(QIcon(":/images/logo.png"));
systemTrayIcon->setToolTip(tr("Project Management System"));
systemTrayIcon->show();
```

#### verifierDeadlinesProjets()
**Added automatic notification:**
```cpp
// Show Windows system notification if there are urgent projects
if (nombreNotifications > 0)
{
    afficherNotificationsWindows();
}
```

#### New Function: afficherNotificationsWindows()
**Purpose:** Display Windows toast notifications for urgent projects

**Features:**
- Checks if system tray notifications are supported
- Queries database for urgent projects (deadline ≤ 7 days)
- Categorizes projects by urgency:
  - ❌ EXPIRED (past deadline)
  - 🔴 DUE TODAY
  - ⏰ DUE WITHIN 7 DAYS
- Shows summary counts for each category
- Displays details for first 3 urgent projects
- Uses appropriate notification icon based on urgency level

## How It Works

### Notification Triggers
Windows notifications appear automatically when:
1. You navigate to the Project Management page
2. The system detects projects with deadlines ≤ 7 days
3. The `verifierDeadlinesProjets()` function is called

### Notification Levels

#### 🔴 Critical (Expired Deadlines)
```
Title: ⚠️ URGENT: Deadlines Expired!
Icon: Critical/Error icon
Message: Shows expired projects with days overdue
```

#### ⚠️ Warning (Due Today)
```
Title: 🔴 Deadlines Due Today!
Icon: Warning icon
Message: Shows projects due today
```

#### ℹ️ Information (Upcoming)
```
Title: ⏰ Upcoming Deadlines
Icon: Information icon
Message: Shows projects due within 7 days
```

## Example Notifications

### Example 1: Mixed Urgency
```
┌─────────────────────────────────────────┐
│ ⚠️ URGENT: Deadlines Expired!          │
├─────────────────────────────────────────┤
│ ❌ 1 project(s) EXPIRED                │
│ 🔴 2 project(s) due TODAY              │
│ ⏰ 3 project(s) due within 7 days      │
│                                         │
│ Total: 6 urgent project(s)             │
│                                         │
│ • Website Redesign: EXPIRED 2 days ago │
│ • Mobile App: Due TODAY!               │
│ • Database Migration: Due in 3 days    │
│                                         │
│ ... and 3 more                         │
└─────────────────────────────────────────┘
```

### Example 2: Only Upcoming Deadlines
```
┌─────────────────────────────────────────┐
│ ⏰ Upcoming Deadlines                   │
├─────────────────────────────────────────┤
│ ⏰ 2 project(s) due within 7 days      │
│                                         │
│ Total: 2 urgent project(s)             │
│                                         │
│ • Client Presentation: Due in 2 days   │
│ • Marketing Campaign: Due in 5 days    │
└─────────────────────────────────────────┘
```

### Example 3: Critical Today
```
┌─────────────────────────────────────────┐
│ 🔴 Deadlines Due Today!                │
├─────────────────────────────────────────┤
│ 🔴 1 project(s) due TODAY              │
│                                         │
│ Total: 1 urgent project(s)             │
│                                         │
│ • Product Launch: Due TODAY!           │
└─────────────────────────────────────────┘
```

## Notification Details

**Display Duration:** 10 seconds (10000 milliseconds)

**Maximum Projects Shown:** First 3 urgent projects (with "...and X more" if there are more)

**Notification Location:** 
- Windows 10/11: Bottom-right corner of screen
- Stored in Windows Action Center for later viewing

## Benefits

### ✅ Advantages of Windows Notifications:

1. **Non-Intrusive**
   - Appears as a toast notification
   - Doesn't block the application
   - Can be dismissed easily

2. **System Integration**
   - Uses native Windows notification system
   - Appears in Windows Action Center
   - Can be reviewed later

3. **No UI Clutter**
   - No in-app panels or popups needed
   - Keeps the interface clean
   - User can choose to view or dismiss

4. **Automatic Alerts**
   - Triggered automatically when deadline conditions are met
   - No manual intervention required
   - Works in the background

5. **Visual Priority**
   - Different icons for different urgency levels
   - Color coding in Windows notifications
   - Easy to spot critical vs informational alerts

## User Experience

### When Notifications Appear:
1. User opens the application
2. User navigates to Project Management page
3. System checks for urgent deadlines
4. If urgent projects found → Windows notification appears
5. User sees toast notification in bottom-right corner
6. Notification stays for 10 seconds then moves to Action Center

### Accessing Old Notifications:
- Click Windows Action Center icon (bottom-right taskbar)
- View recent notifications
- Click notification to bring app to focus

## Technical Details

### System Requirements:
- Windows 7 or later
- Qt with QSystemTrayIcon support
- System tray enabled

### Fallback Behavior:
If system notifications are not supported:
- Function checks `QSystemTrayIcon::supportsMessages()`
- Logs debug message if not supported
- Gracefully exits without error

### Database Query:
```sql
SELECT CODE, TITRE, DEADLINE 
FROM PROJET 
WHERE DEADLINE < SYSDATE 
   OR DEADLINE <= SYSDATE + 7 
ORDER BY DEADLINE ASC
```

## Customization Options

You can modify these parameters in the code:

**Notification Duration:**
```cpp
systemTrayIcon->showMessage(..., 10000);  // Change 10000 to desired milliseconds
```

**Number of Projects Shown:**
```cpp
while (query.next() && displayed < 3)  // Change 3 to show more/fewer projects
```

**Deadline Range:**
```cpp
const QDate dans7Jours = aujourdhui.addDays(7);  // Change 7 to different number of days
```

## Testing

To test the notifications:
1. Create a project with a deadline within 7 days
2. Navigate to Project Management page
3. Windows notification should appear automatically
4. Check Windows Action Center to see notification history

## Troubleshooting

**Problem:** No notifications appear
**Solution:**
- Check Windows notification settings
- Ensure notifications are enabled for the application
- Verify system tray icon is visible in taskbar

**Problem:** Notifications not showing in Action Center
**Solution:**
- Check Windows Settings → System → Notifications
- Enable "Show notification banners" for the app

**Problem:** Console shows "System tray notifications not supported"
**Solution:**
- System doesn't support notifications
- Use the in-app notification panel instead
- Consider upgrading Windows version

═══════════════════════════════════════════════════════════════
The application now shows Windows system notifications instead 
of relying solely on in-app notification panels!
═══════════════════════════════════════════════════════════════
