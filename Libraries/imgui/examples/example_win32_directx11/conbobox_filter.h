//#include <string>
//#include "imgui.h"
//
//namespace ImGui {
//    extern sf::Keyboard::Key; //This key is set by SFML. I had trouble using the correct ImGui Keys, since I only got Count as Key. See the TODOs in propose()
//
//
//    bool identical(const char* buf, const char* item) {
//        size_t buf_size = strlen(buf);
//        size_t item_size = strlen(item);
//        //Check if the item length is shorter or equal --> exclude
//        if (buf_size >= item_size) return false;
//        for (int i = 0; i < strlen(buf); ++i)
//            // set the current pos if matching or return the pos if not
//            if (buf[i] != item[i]) return false;
//        // Complete match
//        // and the item size is greater --> include
//        return true;
//    };
//
//    int propose(ImGuiInputTextCallbackData* data) {
//        //We don't want to "preselect" anything
//        if (strlen(data->Buf) == 0) return 0;
//
//        //Get our items back
//        const char** items = static_cast<std::pair<const char**, size_t>*> (data->UserData)->first;
//        size_t length = static_cast<std::pair<const char**, size_t>*> (data->UserData)->second;
//
//        //We need to give the user a chance to remove wrong input
//        //We use SFML Keycodes here, because the Imgui Keys aren't working the way I thought they do...
//        if (key == sf::Keyboard::BackSpace) { //TODO: Replace with imgui key
//            //We delete the last char automatically, since it is what the user wants to delete, but only if there is something (selected/marked/hovered)
//            //FIXME: This worked fine, when not used as helper function
//            if (data->SelectionEnd != data->SelectionStart)
//                if (data->BufTextLen > 0) //...and the buffer isn't empty			
//                    if (data->CursorPos > 0) //...and the cursor not at pos 0
//                        data->DeleteChars(data->CursorPos - 1, 1);
//            return 0;
//        }
//        if (key == sf::Keyboard::Key::Delete) return 0; //TODO: Replace with imgui key
//
//
//
//        for (int i = 0; i < length; i++) {
//            if (identical(data->Buf, items[i])) {
//                const int cursor = data->CursorPos;
//                //Insert the first match
//                data->DeleteChars(0, data->BufTextLen);
//                data->InsertChars(0, items[i]);
//                //Reset the cursor position
//                data->CursorPos = cursor;
//                //Select the text, so the user can simply go on writing
//                data->SelectionStart = cursor;
//                data->SelectionEnd = data->BufTextLen;
//                break;
//            }
//        }
//        return 0;
//    }
//
//
//    bool TextInputComboBox(const char* id, std::string& str, size_t maxInputSize, const char* items[], size_t item_len, short showMaxItems) {
//        if (str.size() > maxInputSize) { // too large for editing
//            ImGui::Text(str.c_str());
//            return false;
//        }
//
//        std::string buffer(str);
//        buffer.resize(maxInputSize);
//        bool changed = ImGui::TextInputComboBox(id, &buffer[0], maxInputSize, items, item_len, showMaxItems);
//        // using string as char array
//        if (changed) {
//            auto i = buffer.find_first_of('\0');
//            str = buffer.substr(0u, i);
//        }
//        return changed;
//    }
//
//    // Creates a ComboBox with free text inout and completion proposals
//    // Pass your items via items
//    // showMaxItems determines how many items are shown, when the dropdown is open; if 0 is passed the complete list will be shown; you will want normaly a value of 8
//    bool TextInputComboBox(const char* id, char* buffer, size_t maxInputSize, const char* items[], size_t item_len, short showMaxItems) {
//        //Check if both strings matches
//        if (showMaxItems == 0)
//            showMaxItems = item_len;
//
//        ImGui::PushID(id);
//        std::pair<const char**, size_t> pass(items, item_len); //We need to pass the array length as well
//
//        bool ret = ImGui::InputText("##in", buffer, maxInputSize, ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackAlways, propose, static_cast<void*>(&pass));
//
//
//        ImGui::OpenPopupOnItemClick("combobox"); //Enable right-click
//        ImVec2 pos = ImGui::GetItemRectMin();
//        ImVec2 size = ImGui::GetItemRectSize();
//
//        ImGui::SameLine(0, 0);
//        if (ImGui::ArrowButton("##openCombo", ImGuiDir_Down)) {
//            ImGui::OpenPopup("combobox");
//        }
//        ImGui::OpenPopupOnItemClick("combobox"); //Enable right-click
//
//        pos.y += size.y;
//        size.x += ImGui::GetItemRectSize().x;
//        size.y += 5 + (size.y * showMaxItems);
//        ImGui::SetNextWindowPos(pos);
//        ImGui::SetNextWindowSize(size);
//        if (ImGui::BeginPopup("combobox", ImGuiWindowFlags_::ImGuiWindowFlags_NoMove)) {
//
//            ImGui::Text("Select one item or type");
//            ImGui::Separator();
//            for (int i = 0; i < item_len; i++)
//                if (ImGui::Selectable(items[i]))
//                    strcpy_s(buffer, strlen(items[i]), items[i]);
//
//            ImGui::EndPopup();
//        }
//        ImGui::PopID();
//
//        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
//        ImGui::Text(id);
//
//        return ret;
//    }
//}
