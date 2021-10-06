#include<iostream>
#include<string>
#include<string.h>
#include<fstream>
#include<stdlib.h>
#include<iomanip>
#include <conio.h>
//author: Atnafu Jembere
using namespace std;

struct contact {
	char name[50];
	char phone[15];
	char email[50];
};

struct index { //holds only fields to be used in search in memory(for efficient use of memory). May not be necessary in here
	char name[50];
	char phone[15];
	long  position; //position of the record on the file
};

struct phoneBook {
	index * contacts; //indexes for our file
	int avail; //we use dynamic memory. this stands for the available space for adding new contact
	int noContacts;
};

struct result {  //used to hold search result
	long * results;
	int avail;
	int noResults;
};

void welcome ();
void fareWall ();
int strnCmpi (char *, char *, int);
int openFile (fstream &, char *);
template <class T>
int resizeArr (T * &, long, int);
void sort (phoneBook);
int loading (fstream &, phoneBook &);
void display (fstream &, phoneBook, result);
void display (fstream &, phoneBook);
void display (contact);
void GetContactInfo (char *, char * input, int);
contact getContact ();
int addContact (fstream &, phoneBook &);
int searchByName (phoneBook, char *, result &);
long searchByPhone (phoneBook, char *, result &);
void search (phoneBook book, result &, char *);
int getEditContact (contact &);
void edit (fstream &, phoneBook, long, contact);
long selectEditContact (fstream &, phoneBook, result, int);
int editContact (fstream &, phoneBook);
int deleteContact (fstream &, phoneBook);
void viewBySearchHandler (fstream &, phoneBook);
void addContactHandler (fstream &, phoneBook &);
void editContactHandler (fstream &, phoneBook);
void deleteContactHandler (fstream &, phoneBook);
void removeDeleted (fstream &, char *);
int mainMenu ();
int editMenu ();
int viewMenu ();
int viewHandler (fstream &, phoneBook);
int editHandler (fstream &, phoneBook &);
void mainHandler (fstream &, phoneBook &);
int main ();
const contact BLANK_CONTACT = { "","","" };

void welcome () {
	cout << "welcome To phone book application." << endl;;
	cout << "This application allow you to mange your contacts.";
	system ("pause");
}

void fareWall () {
	system ("cls");
	cout << "\nThank you for using our contact book application!!! Goodbye.\n";
}

int strnCmpi (char * first, char * second, int withLen = INT_MAX) {
	int lenFirst = strlen (first);
	int lenSecond = strlen (second);
	int i = 0;
	int minLen = lenFirst < lenSecond ? lenFirst : lenSecond;
	while (i < minLen && i < withLen) {
		if (tolower (first[i]) > tolower (second[i]))
			return 1;
		else if (tolower (first[i]) < tolower (second[i]))
			return -1;
		i++;
	}
	return 0;
}

int openFile (fstream & fstr, char *path) { //open the file it exist. if not ,it will be created.
	fstr.open (path, ios::in | ios::out | ios::binary);
	if (fstr)
		return 1;
	else {
		ofstream ofs (path, ios::out | ios::binary);
		if (ofs) {
			ofs.close ();
			return openFile (fstr, path);
		}
		else
			return 0;
	}
}

template <class T>
int resizeArr (T * & oldArr, long oldSize, int increment) { //resize the dynamic array
	T * temp;
	temp = new (nothrow)T[oldSize + increment];
	if (temp) {
		for (long k = 0; k < oldSize; k++)
			temp[k] = oldArr[k];
		delete[]oldArr;
		oldArr = temp;
		return 1;
	}
	return 0;

}

void sort (phoneBook  book) { //selection sort is used
	long minPos;
	index con;
	for (long i = 0; i < book.noContacts - 1; i++) {
		minPos = i;
		for (long j = i + 1; j < book.noContacts; j++) {
			if (strnCmpi (book.contacts[j].name, book.contacts[minPos].name) < 0)
				minPos = j;
		}
		con = book.contacts[i];
		book.contacts[i] = book.contacts[minPos];
		book.contacts[minPos] = con;
	}
}

long noOfContact (fstream &fstr) { //determines the number of contact on the file
	fstr.clear ();
	fstr.seekg (0, ios::end);
	return (fstr.tellg () / sizeof (contact));
}

int loading (fstream &fstr, phoneBook &  book) { //loads from file into memory
	long m = 0, pos;
	contact con;
	m = noOfContact (fstr);
	book.contacts = new (nothrow) index[m + 10];
	if (book.contacts) {
		int n = 0;
		fstr.seekg (0, ios::beg);
		for (long i = 0; i < m; i++) {
			pos = fstr.tellg ();
			fstr.read ((char *)&con, sizeof (con));
			if (strcmp (con.name, "") != 0) {
				strncpy_s (book.contacts[n].name, con.name, 49);
				strncpy_s (book.contacts[n].phone, con.phone, 14);
				book.contacts[n].position = pos;
				n++;
			}
		}
		book.noContacts = n;
		book.avail = m + 10 - n;
		if (n != 0) sort (book);
		return 1;
	}
	else
		return 0;

}

void display (fstream &fstr, phoneBook book, result res) { //display search result
	system ("cls");
	contact con;
	if (res.noResults == 0) cout << "No match found!!!" << endl;
	else {
		cout << "Search Result Contacts List" << endl;
		cout << setw (4) << left << "#" << ' ' << setw (50) << left << "Full Name" << ' ' << setw (15) << left << "Phone Number" << ' ' << setw (50) << left << "Email" << endl;
		for (long i = 0; i < res.noResults; i++) {
			fstr.seekg (book.contacts[res.results[i]].position, ios::beg);
			fstr.read ((char *)&con, sizeof (con));
			cout << left << setw (4) << i + 1 << ' ' << setw (50) << left << con.name << ' ' << setw (15) << left << con.phone << ' ' << setw (50) << left << con.email << endl;
		}
	}
	fstr.clear ();
}

void display (fstream & fstr, phoneBook book) { //display all contact
	system ("cls");
	contact con;
	long i = 0, j, n;
	n = book.noContacts;

	if (n == 0) cout << "Contact book empty!!!" << endl;
	else {
		cout << "All Contacts List" << endl;
		cout << setw (4) << left << "#" << ' ' << setw (50) << left << "Full Name" << ' ' << setw (15) << left << "Phone Number" << ' ' << setw (50) << left << "Email" << endl;
		j = 0;
		while (i < n) {
			fstr.seekg (book.contacts[i].position, ios::beg);
			fstr.read ((char *)&con, sizeof (con));
			if (strcmp (con.name, "") != 0) {
				cout << left << setw (4) << j++ + 1 << ' ' << setw (50) << left << con.name << ' ' << setw (15) << left << con.phone << ' ' << setw (50) << left << con.email << endl;
			}
			i++;
		}
	}
	fstr.clear ();
}

void display (contact con) { //display single contact
	cout << left << setw (50) << "Full Name: " << '\t' << con.name << endl;
	cout << setw (50) << "Phone number: " << '\t' << con.phone << endl;
	cout << setw (50) << "Email: " << '\t' << con.email << endl;
}

void GetContactInfo (char prompt[], char * input, int maxLength = 50) { //gets single item of contact info from the user
	int valid = 0;
	cout << prompt;
	do {
		cin.getline (input, maxLength);
		if (strcmp (input, "") != 0) {
			valid = 1;
		}
	} while (!valid);
}

contact getContact () { //gets full contact info from user
	system ("cls");
	contact con;
	char fullName[] = "Full Name: ", phoneNo[] = "Phone Number: ", email[] = "Email: ";
	cout << "Enter Contact Information" << endl;
	GetContactInfo (fullName, con.name);
	GetContactInfo (phoneNo, con.phone, 15);
	GetContactInfo (email, con.email);
	return con;
}

int addContact (fstream &fstr, phoneBook & book) { // Add new contact to user
	system ("cls");
	contact newCon;
	if (book.avail == 0) {
		int status = resizeArr (book.contacts, book.noContacts, 10);
		if (!status) {
			cout << "Not enough memory." << endl;
			system ("pause");
			return 0;
		}
		book.avail = 10;
	}
	newCon = getContact ();

	long n = book.noContacts;
	fstr.seekp (0, ios::end);
	book.contacts[n].position = fstr.tellp ();
	cout << fstr.tellp () << endl;
	system ("pause");
	strncpy_s (book.contacts[n].name, newCon.name, 49);
	strncpy_s (book.contacts[n].phone, newCon.phone, 14);
	book.noContacts++;
	fstr.write ((char *)&newCon, sizeof (newCon));
	fstr.flush ();
	book.avail--;
	sort (book);
	return 1;
}

long searchByPhone (phoneBook book, char * phone, result & res) { //searches the phone book by phone(linear search)
	res.results = new (nothrow)long[10];
	if (res.results) {
		res.noResults = 0;
		for (long i = 0; i < book.noContacts; i++) {
			if (strncmp (book.contacts[i].phone, phone, strlen (phone)) == 0) {
				if (strcmp (book.contacts[i].phone, "") != 0) { //make sure that deleted contacts not included
					res.results[res.noResults++] = i;
					res.avail--;
					if (res.avail == 0) {
						if (!resizeArr (res.results, res.noResults, 10)) {
							return 0;
						}
					}
				}
			}
		}
	}
	return 1;
}

int searchByName (phoneBook book, char * name, result & res) { //searches the phone book by name (linear search)
	res.results = new (nothrow)long[10];
	res.avail = 0;
	res.noResults = 0;
	if (res.results) {
		res.avail = 10;
		for (long i = 0; i < book.noContacts; i++) {
			if (strnCmpi (book.contacts[i].name, name, strlen (name)) == 0) {
				if (strcmp (book.contacts[i].name, "") != 0) { //make sure that deleted contacts not included
					res.results[res.noResults++] = i;
					res.avail--;
					if (res.avail == 0) {
						if (!resizeArr (res.results, res.noResults, 10)) {
							return 0;
						}
						res.avail = 10;
					}
				}
			}
		}
	}
	return 1;
}

void search (phoneBook book, result & res, char * prompt) { //search switch board. search is "start with"
	res.results = NULL;
	res.noResults = 0;
	res.avail = 0;
	char target[50];
	int choice;
	system ("cls");
	cout << prompt;
	cin.getline (target, 49);
	if (isdigit (target[0]))
		searchByPhone (book, target, res);
	else
		searchByName (book, target, res);
}

int editContactMenu (contact & oldContact) { //display the menu for edit contact
	int choice;
	system ("cls");
	cout << "Current contact info:" << endl;
	display (oldContact);
	cout << "Which field do you want to edit?" << endl;
	cout << "1. Full Name" << endl;
	cout << "2. Phone number" << endl;
	cout << "3. Email" << endl;
	cout << "4. No more" << endl;
	cout << "Enter your choice: ";
	cin >> choice;
	cin.ignore (INT_MAX, '\n');
	return choice;

}

int getEditContact (contact & oldContact) { //gets a contact for edit from user
	int choice;
	int status = 0;
	char fullName[] = "Full Name: ", phoneNo[] = "Phone Number: ", email[] = "Email: ";
	do {
		choice = editContactMenu (oldContact);
		switch (choice) {
			case 1:
				GetContactInfo (fullName, oldContact.name);
				status = 1; break;
			case 2:
				GetContactInfo (phoneNo, oldContact.phone);
				status = 1; break;
			case 3:
				GetContactInfo (email, oldContact.email);
				status = 1; break;
			case 4:
				break;
			default:
				cout << "Wrong choice!!!" << endl;
		}

	} while (choice != 4);
	system ("cls");
	return status;
}

void edit (fstream &fstr, phoneBook book, long pos, contact newCon) { //edit contact
	strncpy_s (book.contacts[pos].name, newCon.name, 49);
	strncpy_s (book.contacts[pos].phone, newCon.phone, 14);
	fstr.seekp (book.contacts[pos].position, ios::beg);
	fstr.write ((char *)&newCon, sizeof (newCon));
	fstr.flush ();
	sort (book);
	system ("cls");

}

long selectEditContact (fstream & fstr, phoneBook book, result res, int option) { //allow the user to select the user to be edited or deleted
	int choice;
	if (res.noResults == 1) {
		return res.results[0];
	}
	else {
		do {
			system ("cls");
			cout << "Matching contacts" << endl;
			display (fstr, book, res);
			cout << endl;
			if (option == 0)
				cout << "Choose the item number of the contact you want to delete" << endl;
			else
				cout << "Choose the item number of the contact you want to edit" << endl;
			cin >> choice;
			cin.ignore (INT_MAX, '\n');
			if (choice<1 || choice > res.noResults) {
				cout << "Wrong choice!!!";
				system ("pause");
			}
		} while (choice < 1 || choice > res.noResults);
		return res.results[choice - 1];
	}

}

int editContact (fstream &fstr, phoneBook book) { //integrate contact editting
	result res;
	contact con;
	char prompt[] = "Enter name or phone number (partial or full) of the contact to edit (press enter to see all list): ";
	search (book, res, prompt);
	if (res.noResults == 0) {
		system ("cls");
		cout << "No matching contact!!!" << endl;
		return 0;
	}
	long pos = selectEditContact (fstr, book, res, 1);
	delete[] res.results;
	fstr.seekg (book.contacts[pos].position, ios::beg);
	fstr.read ((char *)&con, sizeof (con));
	int status = getEditContact (con);
	if (status) edit (fstr, book, pos, con);
	return status;
}

/*delete contact. For efficiency consideration,the contact is just replaced by blank contact. The actual removal
will be done when the user exits the application*/
int deleteContact (fstream &fstr, phoneBook book) {
	result res;
	char ans;
	contact con;
	char prompt[] = "Enter name or phone number (partial or full) of the contact to delete (press enter to see all list): ";
	search (book, res, prompt);
	system ("cls");
	if (res.noResults == 0) {
		cout << "No matching contact!!!" << endl;
		return 0;
	}
	long pos = selectEditContact (fstr, book, res, 0);
	delete[] res.results;
	fstr.seekg (book.contacts[pos].position, ios::beg);
	fstr.read ((char *)&con, sizeof (con));
	system ("cls");
	cout << "Contact to delete" << endl;
	display (con);
	cout << "Are you sure you want to delete??? Enter y/Y to confirm otherwise any letter." << endl;
	ans = cin.get ();
	system ("cls");
	if (ans == 'y' || ans == 'Y') {
		edit (fstr, book, pos, BLANK_CONTACT);
		return 1;
	}
	else {
		cout << "Not confirmed!!!" << endl;
		return 0;
	}
}

void viewBySearchHandler (fstream &fstr, phoneBook book) { //menu handler
	result res = { NULL,0,0 };
	char prompt[] = "Enter name or phone number (partial or full) of the contact to view (press enter to see all list)): ";
	search (book, res, prompt);
	display (fstr, book, res);
	delete[] res.results;
}

void addContactHandler (fstream &fstr, phoneBook & book) {
	int status = addContact (fstr, book);
	system ("cls");
	if (status)
		cout << "Adding contact successful!!!" << endl;
	else
		cout << "Adding contact failed!!!" << endl;
}

void editContactHandler (fstream &fstr, phoneBook book) {
	int status = editContact (fstr, book);
	if (status)
		cout << "Contact edited successful!!!" << endl;
	else
		cout << "Editing contact failed!!!" << endl;
}

void deleteContactHandler (fstream &fstr, phoneBook book) {
	int status = deleteContact (fstr, book);
	if (status)
		cout << "Contact deleted successfuly!!!" << endl;
	else
		cout << "Delete contact failed!!!" << endl;
}

void removeDeleted (fstream &fstr, char *path) { //removes deleted contact from file when user exits
	contact con;
	ofstream ofs ("temp.bin", ios::out | ios::binary);
	long n = noOfContact (fstr), i = 1;
	fstr.seekg (0, ios::beg);
	while (i <= n) {
		fstr.read ((char *)&con, sizeof (con));
		if (strlen (con.name) != 0) {
			ofs.write ((char *)&con, sizeof (con));
			ofs.flush ();
		}
		i++;
	}
	fstr.close ();
	remove (path);
	ofs.close ();
	rename ("temp.bin", path);
}

int mainMenu () {
	int choice;
	do {
		system ("cls");
		cout << "Main switch board" << endl;
		cout << "1. View" << endl;
		cout << "2. Edit" << endl;
		cout << "3. Exit" << endl;
		cout << "Enter your choice?" << endl;
		cin >> choice;
		cin.ignore (INT_MAX, '\n');
		if (choice < 1 || choice >3) {
			cout << "Wrong choice!!!" << endl;
			system ("pause");
		}
	} while (choice < 1 || choice >3);
	return choice;
}

int editMenu () {
	int choice;
	do {
		system ("cls");
		cout << "Edit board" << endl;
		cout << "1. Add Contact" << endl;
		cout << "2. Edit Contact" << endl;
		cout << "3. Delete Contact" << endl;
		cout << "4. Back to Main" << endl;
		cout << "5. Exit" << endl;
		cout << "Enter your choice?" << endl;
		cin >> choice;
		cin.ignore (INT_MAX, '\n');
		if (choice < 1 || choice >5) {
			cout << "Wrong choice!!!" << endl;
			system ("pause");
		}
	} while (choice < 1 || choice >5);
	return choice;
}

int viewMenu () {
	int choice;
	do {
		system ("cls");
		cout << "View board" << endl;
		cout << "1. View All Contact" << endl;
		cout << "2. View By Search" << endl;
		cout << "3. Back to Main" << endl;
		cout << "4. Exit" << endl;
		cout << "Enter your choice?" << endl;
		cin >> choice;
		cin.ignore (INT_MAX, '\n');
		if (choice < 1 || choice >4) {
			cout << "Wrong choice!!!" << endl;
			system ("pause");
		}
	} while (choice < 1 || choice >4);
	return choice;
}

int viewHandler (fstream &fstr, phoneBook book) {
	int choice;
	do {
		choice = viewMenu ();
		switch (choice) {
			case 1:
				display (fstr, book);
				break;
			case 2:
				viewBySearchHandler (fstr, book);
				break;
			case 3:
				return 1;
			case 4:
				return 0;
			default:

				break;
		}
		if (choice != 1 && choice != 2) {
			cout << "Wrong choice!!!" << endl;
		}
		system ("pause");
	} while (1);
}

int editHandler (fstream & fstr, phoneBook & book) {
	int choice;
	do {
		choice = editMenu ();
		switch (choice) {
			case 1:
				addContactHandler (fstr, book);
				break;
			case 2:
				editContactHandler (fstr, book);
				break;
			case 3:
				deleteContactHandler (fstr, book);
				break;
			case 4:
				return 1;
			case 5:
				return 0;
			default:
				cout << "Wrong choice!!!" << endl;
				break;

		}
		if (choice != 1 && choice != 2 && choice != 3) {
			cout << "Wrong choice!!!" << endl;
		}
		system ("pause");
	} while (1);
}

void mainHandler (fstream & fstr, phoneBook & book) {
	int resp = 1, choice;
	do {
		choice = mainMenu ();
		switch (choice) {
			case 1:
				resp = viewHandler (fstr, book);
				break;
			case 2:

				resp = editHandler (fstr, book);
				break;
			case 3:
				return;
				break;
			default:
				cout << "Wrong choice!!!" << endl;
				system ("pause");
				break;
		}
		if (!resp) return;
	} while (1);
}

int main () {
	phoneBook book = { NULL,0,0 };
	fstream fstr;
	char path[] = "phonebook";
	welcome ();
	int status = openFile (fstr, path);

	if (!status) {
		cout << "Phonebook file corrupted or disk failure!!!" << endl;
	}
	else {
		loading (fstr, book);
		mainHandler (fstr, book);
		removeDeleted (fstr, path);
	}
	fareWall ();
	return 0;
}
