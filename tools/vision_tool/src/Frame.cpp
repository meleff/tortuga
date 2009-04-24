/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_tool/src/App.cpp
 */

// This include must be first because of Mac header defines
#include "vision/include/OpenCVCamera.h"
#include "vision/include/FFMPEGCamera.h"

// Library Includes
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/timer.h>
#include <wx/button.h>

// Project Includes
#include "Frame.h"
#include "GLMovie.h"
#include "MediaControlPanel.h"
#include "DetectorControlPanel.h"
#include "Model.h"

namespace ram {
namespace tools {
namespace visionvwr {

BEGIN_EVENT_TABLE(Frame, wxFrame)
    EVT_MENU(ID_Quit, Frame::onQuit)
    EVT_MENU(ID_About, Frame::onAbout)
    EVT_MENU(ID_OpenFile, Frame::onOpenFile)
    EVT_MENU(ID_OpenCamera, Frame::onOpenCamera)
END_EVENT_TABLE()


Frame::Frame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame((wxFrame *)NULL, -1, title, pos, size),
    m_mediaControlPanel(0),
    m_movie(0),
    m_model(new Model)
{
    // File Menu
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_OpenFile, _T("Open Video &File"));
    menuFile->Append(ID_OpenCamera, _T("Open CV &Camera"));
    menuFile->Append(ID_About, _T("&About..."));
    menuFile->AppendSeparator();
    menuFile->Append(ID_Quit, _T("E&xit"));
    
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _T("&File") );
    
    SetMenuBar( menuBar );

    // Create out controls
    m_mediaControlPanel = new MediaControlPanel(m_model, this);
    m_movie=new GLMovie(this, m_model);
    wxButton* detectorHide = new wxButton(this, wxID_ANY, 
					  wxT("Show/Hide Detector"));
					  
    // Place controls in the sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(m_mediaControlPanel, 0, wxEXPAND, 0);

    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(detectorHide, 0, wxALL, 3);
    //row->Add(m_choice, 1, wxALIGN_CENTER | wxALL, 3);
    sizer->Add(row, 0, wxEXPAND, 0);

    sizer->Add(m_movie, 1, wxEXPAND, 0);

    sizer->SetSizeHints(this);
    SetSizer(sizer);

    // Create the seperate frame for the detector panel
    wxPoint framePosition = GetPosition();
    framePosition.x += GetSize().GetWidth();
    wxSize frameSize(GetSize().GetWidth()/2, GetSize().GetHeight());

    m_detectorFrame = 
      new wxFrame(this, wxID_ANY, _T("Detector Control"), framePosition, 
		  frameSize);

    // Add a sizer and the detector control panel to it
    sizer = new wxBoxSizer(wxVERTICAL);
    wxPanel* detectorControlPanel = new DetectorControlPanel(m_model, 
							     m_detectorFrame);
    sizer->Add(detectorControlPanel, 0, wxEXPAND, 0);

    m_detectorFrame->SetSizer(sizer);
    m_detectorFrame->Show();

    // Register for events
    Connect(detectorHide->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(Frame::onShowHideDetector));
}

void Frame::onQuit(wxCommandEvent& WXUNUSED(event))
{
    // Stop video playback when we shut down
    m_model->stop();    
    Close(true);
}
    
void Frame::onAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("An application to view live or recored video"),
                 _T("About Vision Viewer"), wxOK | wxICON_INFORMATION, this);
}

void Frame::onOpenFile(wxCommandEvent& event)
{
    wxString filename = wxFileSelector(_T("Choose a video file to open"));
    if ( !filename.empty() )
    {
        m_model->openFile(std::string(filename.mb_str()));
    }
}
    
void Frame::onOpenCamera(wxCommandEvent& event)
{
    m_model->openCamera();
}    

void Frame::onShowHideDetector(wxCommandEvent& event)
{
    // Toggle the shown status of the frame
    m_detectorFrame->Show(!m_detectorFrame->IsShown());
}

} // namespace visionvwr
} // namespace tools
} // namespace ram
