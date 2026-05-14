cmake_minimum_required(VERSION 3.16)

# Project setup
project(MedicineReminder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# QT CONFIGURATION
# -----------------------------------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# -----------------------------------------------------------------------------
# SOURCE FILES
# -----------------------------------------------------------------------------
set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui

    # Core Logic
    Medicine.cpp
    Medicine.h
    MedicineDatabase.cpp
    MedicineDatabase.h
    ConflictChecker.cpp
    ConflictChecker.h
    MissedDoseLog.cpp
    MissedDoseLog.h
    Chatbot.h

    # UI Widgets
    DailyLog.cpp
    DailyLog.h
    DashboardWidget.cpp
    DashboardWidget.h
    MedicinesWidget.cpp
    MedicinesWidget.h
    ScheduleWidget.cpp
    ScheduleWidget.h
    ConflictsWidget.cpp
    ConflictsWidget.h
    MissedWidget.cpp
    MissedWidget.h
    ChatbotWidget.cpp
    ChatbotWidget.h

    # Dialogs
    MedicineDialog.cpp
    MedicineDialog.h
    ReminderDialog.cpp
    ReminderDialog.h
)

# Optional: Add resources.qrc only if it exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc")
    list(APPEND PROJECT_SOURCES resources.qrc)
endif()

# -----------------------------------------------------------------------------
# EXECUTABLE GENERATION
# -----------------------------------------------------------------------------
# FIX IS HERE: Removed MANUAL_FINALIZATION
add_executable(MedicineReminder
    ${PROJECT_SOURCES}
    DailyLog.h DailyLog.cpp
)

target_link_libraries(MedicineReminder PRIVATE Qt6::Widgets)

# -----------------------------------------------------------------------------
# COPY DATA FILES (Optional)
# -----------------------------------------------------------------------------
file(GLOB DATA_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/medicines.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/conflicts.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/missed.txt"
)

if(DATA_FILES)
    file(COPY ${DATA_FILES} DESTINATION ${CMAKE_BINARY_DIR})
endif()
