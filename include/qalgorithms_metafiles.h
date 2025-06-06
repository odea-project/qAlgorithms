// qalgorithms_metafiles.h
#ifndef QALGORITHMS_META_H
#define QALGORITHMS_META_H

#include <vector>
#include <filesystem>

namespace qAlgorithms
{
    /*
    The tasklist is the standalone interface into qAlgorithms. It allows the user
    to organise individual files via a small scripting interface if needed, although the
    required entries can be generated using the program itself. The tasklist stores
    commands and data, but mostly data. This information is organised in the form of
    TaskItems, which are either of subtype action or data. Every TaskItem is stored
    in a vector per type, while the tasklist is implemented as a linked list. This
    offers higher general flexibility in reordering entries etc., while also having
    less issues with inserting follow-up actions automatically. (Example: automatically
    create two replicate groups if replicates were measured in mixed mode).
    */

    enum dataType
    {
        undefined,
        sample,
        quality_control,
        standards,
        blank_instrument,
        logfile,
        outdir
    };

    enum actionType
    { // most of these are just examples for the time being @todo
        initalisation,
        output,
        logging,
        new_replicate,
        new_blank,
        new_standard,
        new_QC,
        filter_replicates,
        substract_blank,
        recall_standards,
        end // is this necessary?
    };

    struct TaskItem_data
    {
        // contains both path and string to have validation externalised from the task list contruction
        std::filesystem::path path;
        std::string content;

        size_t accessIndex;

        int replicateID;
        dataType type;
        bool polarity;
        bool nextIsData;
    };

    struct TaskItem_action
    {
        std::string content;
        const actionType type;
        // see above
        size_t accessIndex;
        bool nextIsData;
    };

    // reads the file and does minimal error handling. If the return vector is empty,
    // file read in failed. Otherwise, it is input for taskListSetup. taskfiles are
    // supplied by the user directly or some other interface program
    std::vector<std::string> readTaskList_txt(std::filesystem::path taskfile_txt);

    // this fills the two empty vectors actions and data with the user input taken from the tasklist
    // the tasks are linked to each other, but all other structure such as tagging is handled only
    // when validating the input
    void taskListSetup(std::vector<TaskItem_action> *actions,
                       std::vector<TaskItem_data> *data,
                       const std::vector<std::string> *input);

    // @todo check for necessity
    void runTaskAction(const TaskItem_action item);

}

#endif