#include "qalgorithms_metafiles.h"

#include <vector>
#include <iostream>
#include <cassert>

namespace qAlgorithms
{
    /*
    incomplete task list syntax:
    # // the hashtag comments a line out
    BEGIN // first entry of the tasklist - all entries before this one are deleted!
    OUT // path the output should be written to is given in the next line
    LOGFILE "(path)" // write process stats to a logfile, the path a logfile should be written to is given in the next line
    REPLICATE // all data until the next replicate are replicate measurements of a sample. @todo add plausibility check
    REPLICATE_N // same as replicate, but give a name @todo option to disable plausibility check
    STANDARD // all data until the next data action are standards
    BLANK // all data until the next data action are blanks
    QC // all data until the next data action are quality control samples
    */

    void taskListSetup(std::vector<TaskItem_action> *actions,
                       std::vector<TaskItem_data> *data,
                       const std::vector<std::string> *input) // @todo always push a terminator to the back of input to prevent bad access
    {
        if (!actions->empty())
        {
            std::cerr << "Error: the tasklist contains actions before initialisation\n";
        }
        if (!data->empty())
        {
            std::cerr << "Error: the tasklist contains data before initialisation\n";
        }
        if (input->empty())
        {
            std::cerr << "Error: no input supplied to function taskListSetup\n";
        }

        size_t previousPosition = 0;
        bool previousWasData = false;

        // lambda function to update positions of the previous entry after adding an element
        auto updateLink = [&previousPosition, &previousWasData, actions, data](size_t newPosition, bool newIsData)
        {
            if (previousWasData)
            {
                data->at(previousPosition).nextIsData = newIsData;
                data->at(previousPosition).accessIndex = newPosition;
            }
            else
            {
                actions->at(previousPosition).nextIsData = newIsData;
                actions->at(previousPosition).accessIndex = newPosition;
            }

            // also updates the bookkeeping variables
            previousPosition = newPosition;
            previousWasData = newIsData;
        };

        for (size_t i = 0; i < input->size(); i++)
        {
            auto entry = input->at(i);
            if (entry.empty()) // empty line or whitespace @todo during parsing
            {
                continue;
            }
            if (entry[0] == '#') // line was commented out
            {
                continue;
            }
            if (entry == "BEGIN")
            {
                // reasoning: this implementation allows you to skip processing entries
                // with relatively fine control without adding a "skip" launch option
                actions->clear();
                data->clear();
                previousPosition = 0;
                previousWasData = false;
                actions->push_back({"",
                                    initalisation,
                                    0,
                                    false});
            }
            else if (entry == "LOGFILE")
            {
                // @todo add a logging option that actually makes sense from a user perspective.
                // this is not equivalent to the processing stats we will use for quality control -
                // those should not be optional to begin with
                actions->push_back({"", // @todo this is the path!
                                    logging,
                                    0,
                                    false});
                updateLink(actions->size() - 1, false);
            }
            else if (entry == "REPLICATE")
            {
                // @todo add a check using method name and vial position, consider adding another option for non repeat injections
                actions->push_back({"",
                                    new_replicate,
                                    0,
                                    false});
                updateLink(actions->size() - 1, false);
            }
            else if (entry == "STANDARD")
            {
                // the table of standards is given in an unspecified format in the next line
                i += 1;
                actions->push_back({input->at(i),
                                    new_standard,
                                    0,
                                    false});
                updateLink(actions->size() - 1, false);
            }
            else if (entry == "BLANK")
            {
                actions->push_back({"",
                                    new_blank,
                                    0,
                                    false});
                updateLink(actions->size() - 1, false);
            }
            else if (entry == "QC")
            {
                actions->push_back({"",
                                    new_QC,
                                    0,
                                    false});
                updateLink(actions->size() - 1, false);
            }
            else if (entry == "OUT")
            {
                i += 1;
                actions->push_back({input->at(i),
                                    new_QC,
                                    data->size(),
                                    true});
                updateLink(actions->size() - 1, false);
            }
            else
            {
                // if a line is none of the known commands, it must be a file. If it is not,
                // it can only be a mistyped command or path.
                i += 1;
                data->push_back({"", // the path is added only when it was confirmed to exist
                                 input->at(i),
                                 data->size(),
                                 -1,
                                 undefined,
                                 false,
                                 false});
            }
        }
    }
}