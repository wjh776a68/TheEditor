#include "dialog.h"
void dialog::oninitdialog()
{
	m_Menu.LoadMenu(IDC_BLOOMFILTER);
	SetMenu(&m_Menu);
}