#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

#include "MPointer.h"
#include "LinkedList.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void onConnectClicked();
    void onCreateIntPointerClicked();
    void onCreateDoublePointerClicked();
    void onCreateStringPointerClicked();
    void onSetValueClicked();
    void onGetValueClicked();
    void onLinkedListAddClicked();
    void onLinkedListRemoveClicked();
    void onLinkedListDisplayClicked();
    
private:
    // Connection widgets
    QLineEdit *portLineEdit;
    QPushButton *connectButton;
    QLabel *connectionStatusLabel;
    
    // MPointer operation widgets
    QComboBox *pointerSelector;
    QLineEdit *valueLineEdit;
    QPushButton *setValueButton;
    QPushButton *getValueButton;
    QLabel *valueDisplayLabel;
    
    // Create pointer widgets
    QPushButton *createIntButton;
    QPushButton *createDoubleButton;
    QPushButton *createStringButton;
    
    // Linked List widgets
    QLineEdit *listValueLineEdit;
    QPushButton *listAddButton;
    QSpinBox *listIndexSpinBox;
    QPushButton *listRemoveButton;
    QPushButton *listDisplayButton;
    QTextEdit *listDisplayArea;
    
    // Pointer storage
    std::vector<MPointer<int>> intPointers;
    std::vector<MPointer<double>> doublePointers;
    std::vector<MPointer<char[64]>> stringPointers;
    
    // Linked list
    LinkedList<int> intList;
    
    void updatePointerSelector();
    void setupUI();
};