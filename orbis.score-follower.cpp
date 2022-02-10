/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <string>

using namespace c74::min;



class TriggerListener {


public:
    
    
    TriggerListener(){
        m_note = 0;
        m_repeats = 0;
        m_midiActions.clear();
        m_id = 0;
        m_next = false;
        m_hasMidi = false;
    }
    
    TriggerListener(int nb){
        m_note = 0;
        m_repeats = 0;
        m_midiActions.clear();
        m_id = nb;
        m_next = false;
        m_hasMidi = false;
    }

    TriggerListener(int nb, int N, int R, atoms A){
        m_note = N;
        m_repeats = R;
        m_midiActions = A;
        m_id = nb;
        m_next = false;
        m_hasMidi = false;
    }

    
    
        void setNote(int N){
            m_note = N;
        }
        void setRepeats(int R){
            m_repeats = R;
        }

        void setMidiActions(atoms A){
            m_hasMidi = true;
            m_midiActions = A;
        }

        bool hasMidiActions(){return m_hasMidi;}

        void setNextFlag(){
            m_next = true;
        }

        void resetNextFlag(){m_next = false;}

        bool getNextFlag(){
            return m_next;
        }

        // Can probably do without a string conversion
        string actionToString(){
            string s = "";
            if(!m_midiActions.empty()) {
                for (atom a : m_midiActions) {
                    s += string(a) + " ";
                }
            }
            return s;
        }
    

        int getNote(){return m_note;}
        int getRepeats(){return m_repeats;}
        atoms getMidiActions(){return m_midiActions;}
        int getId(){ return m_id;}

    
private:
        int m_note;
        int m_repeats;
        atoms m_midiActions;
        int m_id;
        bool m_next;
        bool m_hasMidi;

};






class score_follower : public object<score_follower> {
public:
    MIN_DESCRIPTION	{"Follow a series of MIDI triggers and synchronise with incoming MIDI data"};
    MIN_TAGS		{"utilities"};
    MIN_AUTHOR		{"James Leonard"};
    MIN_RELATED		{"print, jit.print, dict.print"};

    inlet<>  midi_in	{ this, "(list) receive incoming MIDI data" };
    inlet<>  trigger_in    { this, "(anything) receive incoming trigger data" };
    
    outlet<> midi_out    { this, "(list) output triggered MIDI data" };
    outlet<> cellblock    { this, "(list) cellblock visualisation data" };
    outlet<> note_table    { this, "(list) list of all note counters" };
    outlet<> next	{ this, "(bang) next trigger event" };



    // define an optional argument for setting the message
    argument<symbol> greeting_arg { this, "greeting", "Initial value for the greeting attribute.",
        MIN_ARGUMENT_FUNCTION {
            greeting = arg;
        }
    };


    // the actual attribute for the message
    attribute<symbol> greeting { this, "greeting", "Welcome to the orbis score follower",
        description {
            "Greeting to be posted. "
            "The greeting will be posted to the Max console when a bang is received."
        }
    };
    
    
    // Constructor Method
    score_follower(const atoms& args = {}){
        nbHits.fill(0);
        cout << "Doing the constructor stuff" << endl;
        
    }
    
    
    void setMinVel(int val) {
        minVel = val;
    }

    
    void resetNoteCounters() {
        nbHits.fill(0);
        bang();
        cout << "Reset all note counters to zero" << endl;
    }

    void resetNotesAndTriggers() {
        nbHits.fill(0);
        triggers.clear();
        counter = 0;
        bang();
        cout << "Reset all note counters and triggers" << endl;
    }

    
    
    void updateNoteTable(){
        atoms m_data;
        m_data.clear();
        for(auto nb : nbHits)
            m_data.push_back(nb);
        note_table.send(m_data);
    }
    
    
    message<> reset { this, "reset", "Reset all note counters",
        MIN_FUNCTION {
            resetNoteCounters();
            return {};
        }
    };
    
    message<> resetAll { this, "resetAll", "Reset all notes and triggers.",
        MIN_FUNCTION {
            resetNotesAndTriggers();
            return {};
        }
    };
    
    
    
    message<> dump { this, "dump", "Dump all note count information.",
        MIN_FUNCTION {
            cout << "Score Follower - Note Dump:" << endl;
            for(auto i=0; i < nbHits.size(); i++)
                cout << " - Hits for " << i << ": " << nbHits[i] << endl;            return {};
        }
    };
    
    message<> dumpTriggers { this, "dumpTriggers", "Dump current triggers.",
        MIN_FUNCTION {
            if (triggers.empty()) {
                cout << "No armed triggers currently set" << endl;
                return{};
            }
            else for (TriggerListener L : triggers) {
                cout << "Trigger is waiting for :" << endl;
                cout << "   - Note :" << L.getNote() << endl;
                cout << "   - Repeats :" << L.getRepeats() << endl;
                cout << "   - Action :" << L.actionToString()<< endl;
            }
            return {};
        }
    };
    
    

    

    // respond to the bang message to do something
    message<> bang { this, "bang", "Post the greeting.",
        MIN_FUNCTION {
            
            atoms m_data;
            
            updateNoteTable();
            
            // update the cellblock stuff
            m_data.clear();
            m_data.push_back("clear");
            m_data.push_back("all");
            cellblock.send(m_data);
            
            if(triggers.empty()){
                m_data.clear();
                m_data.push_back("set");
                m_data.push_back(0);
                m_data.push_back(0);
                m_data.push_back("empty table");
                cellblock.send(m_data);
            }
            else{
                int i = 0;
                for(auto t : triggers){
                    m_data.clear();
                    m_data.push_back("set");
                    m_data.push_back(0);
                    m_data.push_back(i);
                    std::string s = "Trigger @" + std::to_string(t.getId())
                            + "  |  Note: " + std::to_string(t.getNote())
                            + "  |  completed: " + std::to_string(nbHits[t.getNote()])
                            + "/" + std::to_string(t.getRepeats())
                            + "  |  action: " + t.actionToString();
                    
                    if(t.getNextFlag())
                        s.append("next");
                    
                    m_data.push_back(s);
                    cellblock.send(m_data);
                    i++;
                }
            }
            return {};
        }
    };
    
    

    
    
    
    bool isMidiInt(atom a)
    {
        if(a.type() == c74::min::message_type::int_argument)
            if(int(a) > -1 && int(a) < 128)
                return true;
        return false;
    }
    
    
    bool isPositiveInt(atom a)
    {
        if(a.type() == c74::min::message_type::int_argument)
            if(int(a) > 0)
                return true;
        return false;
    }
    
    message<threadsafe::yes> realnote {
        this, "realnote", "MIDI note information", MIN_FUNCTION {
            
            cout << "got the realnote message " << endl;
            
            int lSize = 0;
            bool nextFlag;
            TriggerListener tmp;
            atoms m_data;
            
            // trigger information
            if(inlet == 1) {
                    
                    if(args.size()<3){
                        cout << "Not enough information to generate trigger data" << endl;
                        return {};
                    }

                    tmp = TriggerListener(counter);

                    // Add the awaited note and repeats to a temporary listener
                    if (isMidiInt(args[0]) && isPositiveInt(args[1])){
                        tmp.setNote(int(args[0]));
                        tmp.setRepeats(int(args[1]));
                        lSize = args.size()-3;
                        nextFlag = false;
                        
                        // of the last element of the triggger calls for "next" then raise the flag
                        if(args[args.size()-1] == "next"){
                            cout << "found next trigger" << endl;
                            tmp.setNextFlag();
                            lSize = args.size()-4;
                            nextFlag = true;
                        }
                        
                        // if there is MIDI information associated with trigger
                        if(args[2] == "trigger"){
                            m_data.clear();
                            for(auto i = 0; i < lSize; i++)
                                m_data.push_back(args[3+i]);
                            tmp.setMidiActions(m_data);
                            triggers.push_back(tmp);
                            counter++;
                        }
                        else if (nextFlag){
                            triggers.push_back(tmp);
                            counter++;
                        }
                        else
                            cout << "unknown message type" << endl;
                    }
                    else
                        cout << "Bad format for trigger information" << endl;
                    
                    // update outputs
                    bang();
                
            }
            return {};
        }
    };

    
    message<threadsafe::yes> list {
        this, "list", "MIDI note information", MIN_FUNCTION {
            
            int note = 0, vel = 0;
            bool refresh;
            TriggerListener tmp;
            atoms m_data;
            
            switch (inlet) {
                    
                // incoming midi notes
                case 0:
                    
                    // Process the new incoming note information
                    refresh = false;
                    if ((args.size() == 2) && isMidiInt(args[0]) && isMidiInt(args[1])){
                        
                        note = int(args[0]);
                        vel = int(args[1]);
                        
                        if(vel > minVel){
                            nbHits[note] += 1;
                            // only as to update the trigger table if there are changes to tracked notes
                            for (TriggerListener L : triggers) {
                                if (note == L.getNote())
                                    refresh = true;
                            }
                            updateNoteTable();
                        }
                        
                        // Now see if any triggers have been activated
                        if (triggers.size() > 0) {
                            // Go through trigger list, and find if one is ready
                            for (int i = triggers.size() - 1; i >= 0; i--) {
                                
                                TriggerListener L = triggers.at(i);

                                if (nbHits[L.getNote()] >= L.getRepeats()) {
                                    if (L.hasMidiActions()) {
                                        midi_out.send(L.getMidiActions());
                                        cout << "Trigger event for note " << L.getNote() << endl;
                                        triggers.erase(triggers.begin()+i);
                                    }
                                    if (L.getNextFlag()) {
                                        resetAll();
                                        next.send("bang");
                                        cout << "Event for next trigger file" << endl;
                                        // jump out of loop if we've deleted all triggers!
                                        break;
                                    }
                                    refresh = true;
                                }
                            }
                        }
                        if (refresh)
                            bang();
                    }
                    else
                        cout << "Incoming info does not respect MIDI note/vel format" << endl;
                    
                    break;
 
                default:
                    assert(false);
            }
            return {};
        }
    };
    
    

    // post to max window == but only when the class is loaded the first time
    message<> maxclass_setup { this, "maxclass_setup",
        MIN_FUNCTION {
            cout << "Orbis Score Follower: Trigger Listener System" << endl;
            return {};
        }
    };
    
    
private:
    
    std::vector<TriggerListener> triggers;
    std::array<int, 128> nbHits;
    
    int counter = 0;
    int minVel = 1;

    

};


MIN_EXTERNAL(score_follower);
