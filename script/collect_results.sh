#!/bin/bash

mode=$1         # CP1, CP2, MIP, HEU

attempt=$2      # attempt

time_limit=300                          # Time limit in second for the test

input_data_folder=input_data/           # input data folder

if [ -z "$mode" ]; then
    echo "Missing solver mode"
    exit 1
elif [[ ! "$mode" =~ "CP1" && ! "$mode" =~ "CP2" && ! "$mode" =~ "MIP" && ! "$mode" =~ "HEU" && ! "$mode" =~ "LS" && ! "$mode" =~ "PB" && ! "$mode" =~ "SA" && ! "$mode" =~ "HC" && ! "$mode" =~ "Tabu" && ! "$mode" =~ "LS2" && ! "$mode" =~ "PB2" && ! "$mode" =~ "SA2" && ! "$mode" =~ "HC2" && ! "$mode" =~ "Tabu2" && ! "$mode" =~ "BFSA" ]]; then
    echo "Invalid solver mode"
    exit 1  
else
    if [ $mode == "HEU" ]; then 
        output_folder=results/results_${mode} 

        files=$(ls $input_data_folder/*.txt)                        # List all *.txt files in input folder

        output_csv=$output_folder/results_${mode}_$attempt.csv      # csv file for results
    elif [ $mode == "LS" ]; then 
        output_folder=results/results_${mode} 

        files=$(ls $input_data_folder/*.txt)           

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv 
    elif [ $mode == "PB" ]; then 
        output_folder=results/results_${mode} 

        files=$(ls $input_data_folder/*.txt)        

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv 
    elif [ $mode == "SA" ]; then 
        output_folder=results/results_${mode} 

        files=$(ls $input_data_folder/*.txt)        

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv 
    elif [ $mode == "HC" ]; then 
        output_folder=results/results_${mode} 

        files=$(ls $input_data_folder/*.txt)        

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv 
    elif [ $mode == "Tabu" ]; then 
        output_folder=results/results_${mode} 

        files=$(ls $input_data_folder/*.txt)        

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv
    elif [ $mode == "LS2" ]; then 
        output_folder=results/results_LS2 

        files=$(ls $input_data_folder/*.txt)           

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv 
    elif [ $mode == "PB2" ]; then 
        output_folder=results/results_PB2 

        files=$(ls $input_data_folder/*.txt)        

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv 
    elif [ $mode == "SA2" ]; then 
        output_folder=results/results_SA2 

        files=$(ls $input_data_folder/*.txt)        

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv 
    elif [ $mode == "HC2" ]; then 
        output_folder=results/results_HC2 

        files=$(ls $input_data_folder/*.txt)        

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv 
    elif [ $mode == "Tabu2" ]; then 
        output_folder=results/results_Tabu2 

        files=$(ls $input_data_folder/*.txt)        

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv
    elif [ $mode == "BFSA" ]; then 
        output_folder=results/results_BFSA 

        files=$(ls $input_data_folder/*.txt)        

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv
    else
        output_folder=results/results_${mode}                     

        files=$(ls $input_data_folder/*.txt)     

        output_csv=$output_folder/results_${mode}_${time_limit}_$attempt.csv 
    fi

    mkdir -p $output_folder                                        
fi

# Create the columns for the results file
echo "n_packs, n_bins, n_bins_used, cost, status, solver_running_time, real_running_time, time_limit_for_solver," > $output_csv

stats=("Number of bin used" "Total cost" "Status" "Running time" "Real running Time")

for file in $files; do
    # Run the solver 
    echo "Working with $(basename $file)"
    if [ $mode == "CP1" ]; then 
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  python solver_file/CP_model_solver/CP_model_1.py $file $time_limit > $output_folder/$(basename $file).out 
    elif [ $mode == "CP2" ]; then 
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  python solver_file/CP_model_solver/CP_model_2.py $file $time_limit > $output_folder/$(basename $file).out 
    elif [ $mode == "MIP" ]; then 
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  python solver_file/MIP_model.py $file $time_limit > $output_folder/$(basename $file).out 
    elif [ $mode == "HEU" ]; then 
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/heuristic_main $file > $output_folder/$(basename $file).out 
    elif [ $mode == "LS" ]; then 
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/local_search_main $file $time_limit > $output_folder/$(basename $file).out 
    elif [ $mode == "PB" ]; then 
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/population_based_main $file $time_limit > $output_folder/$(basename $file).out 
    elif [ $mode == "SA" ]; then 
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/SA_main $file $time_limit > $output_folder/$(basename $file).out
    elif [ $mode == "HC" ]; then
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/hill_climbing $file $time_limit > $output_folder/$(basename $file).out
    elif [ $mode == "Tabu" ]; then
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/tabu_main $file $time_limit > $output_folder/$(basename $file).out
    elif [ $mode == "LS2" ]; then
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/LS2 $file $time_limit > $output_folder/$(basename $file).out
    elif [ $mode == "PB2" ]; then
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/PB2 $file $time_limit > $output_folder/$(basename $file).out
    elif [ $mode == "SA2" ]; then
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/SA2 $file $time_limit > $output_folder/$(basename $file).out
    elif [ $mode == "HC2" ]; then
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/HC2 $file $time_limit > $output_folder/$(basename $file).out
    elif [ $mode == "Tabu2" ]; then
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/tabu2 $file $time_limit > $output_folder/$(basename $file).out
    elif [ $mode == "BFSA" ]; then
        /usr/bin/time -f "Real running Time: %e" -ao $output_folder/$(basename $file).out  ./solver_file/Heuristic/BFSA $file $time_limit > $output_folder/$(basename $file).out
    fi

    # Get the input number of packages and number of bins from input file
    head -n 1 $file | sed -E 's/ /,/g' | tr '\n' ',' >> $output_csv

    # Get stats from the solver, if not return blank 
    for item in "${stats[@]}"; do
        value=$(grep "$item" "$output_folder/$(basename $file).out" | awk '{print $NF}')
        if [[ -z "$value" && "$item" == "Status" ]]; then
            value="NO SOLUTION FOUND"
        fi
        echo "$value" | tr '\n' ',' >> $output_csv
    done
    if  [ $mode == "HEU" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "LS" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "PB" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "SA" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "HC" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "Tabu" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "LS2" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "PB2" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "SA2" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "HC2" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "Tabu2" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    elif [ $mode == "BFSA" ]; then
        echo "None" | tr '\n' ','>> $output_csv
    else
        echo $time_limit | tr '\n' ',' >> $output_csv       # Write the time_limit to result file
    fi

    echo >> $output_csv                                     # Add new line in the results file

    rm $output_folder/$(basename $file).out                 # Remove the temporary output file
done
