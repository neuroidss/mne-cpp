//=============================================================================================================
/**
* @file     compute_fwd_settings.h
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     February, 2017
*
* @section  LICENSE
*
* Copyright (C) 2017, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Compute Forward Setting class declaration.
*
*/

#ifndef COMPUTEFWDSETTINGS_H
#define COMPUTEFWDSETTINGS_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "../fwd_global.h"
#include <fiff/fiff_constants.h>


//*************************************************************************************************************
//=============================================================================================================
// Eigen INCLUDES
//=============================================================================================================

#include <Eigen/Core>


//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QSharedPointer>
#include <QString>


//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE FWDLIB
//=============================================================================================================

namespace FWDLIB
{

//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================


//=============================================================================================================
/**
* Implements the compute forward setting parser
*
* @brief Compute Forward setting implementation
*/
class FWDSHARED_EXPORT ComputeFwdSettings
{
public:
    typedef QSharedPointer<ComputeFwdSettings> SPtr;             /**< Shared pointer type for ComputeFwdSettings. */
    typedef QSharedPointer<const ComputeFwdSettings> ConstSPtr;  /**< Const shared pointer type for ComputeFwdSettings. */

    //=========================================================================================================
    /**
    * Default Constructor
    */
    explicit ComputeFwdSettings();

    //=========================================================================================================
    /**
    * Constructs Compute Forward Settings
    *
    * @param [in] argc (argument count) is an integer that indicates how many arguments were entered on the command line when the program was started.
    * @param [in] argv (argument vector) is an array of pointers to arrays of character objects. The array objects are null-terminated strings, representing the arguments that were entered on the command line when the program was started.
    */
    explicit ComputeFwdSettings(int *argc,char **argv);

    //=========================================================================================================
    /**
    * Destructs the Compute Forward Settings
    */
    virtual ~ComputeFwdSettings();

public:
    QString srcname;            /**< Source space */
    QString measname;           /**< Measurement file */
    QString mriname;            /**< MRI file for head <-> MRI transformation */
    QString transname;          /**< head2mri transformation file */
    bool mri_head_ident;        /**< Are the head and MRI coordinates the same? */
    QString bemname;            /**< BEM model file */
    QString solname;            /**< Solution file */
    QString mindistoutname;     /**< Output file for omitted source space points */
    bool filter_spaces = true;  /**< Filter the source space points */
    Eigen::Vector3f r0;         /**< Sphere model origin  */
    bool accurate = false;      /**< Use accurate calculations */
    bool fixed_ori = false;     /**< Fixed-orientation dipoles? */
    bool include_meg = false;
    bool include_eeg = false;
    bool compute_grad = false;
    QString command;            /**< Saves the recognized command line for future use */
    float mindist = 0.0f;       /**< Minimum allowed distance of the sources from the inner skull surface */
    int coord_frame = FIFFV_COORD_HEAD; /**< Can be changed with the --mricoord option */
    bool do_all = false;
    QStringList labels;         /**< Compute the solution only for these labels */
    int nlabel = 0;

    QString eeg_model_file;     /**< File of EEG sphere model specifications */
    QString eeg_model_name;     /**< Name of the EEG model to use */
    float eeg_sphere_rad = 0.09f;   /**< Scalp radius to use in EEG sphere model */
    bool scale_eeg_pos = false;     /**< Scale the electrode locations to scalp in the sphere model */
    bool use_equiv_eeg = true;      /**< Use the equivalent source approach for the EEG sphere model */
    bool use_threads = true;        /**< Parallelize? */

private:
    void initMembers();
    void usage(char *name);

    static QString build_command_line(QString old, QString new_item);

    bool check_unrecognized_args(int argc, char **argv);
    bool check_args (int *argc,char **argv);

};

//*************************************************************************************************************
//=============================================================================================================
// INLINE DEFINITIONS
//=============================================================================================================


} //NAMESPACE

#endif // COMPUTEFWDSETTINGS_H
