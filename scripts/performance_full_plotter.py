#!/usr/bin/env python3

import re
import csv
import matplotlib.pyplot as plt
import numpy as np
import sys


def plot_error(error_dir_path, file_names):
    plt.figure()
    colors = ["r", "g", "b"]
    titles = ["ABA", "Linear", "None"]
    for i in range(3):
        error_history = []
        targets = []
        elapsed_times = []
        with open(error_dir_path + file_names[i]) as f:
            for line in f.readlines():
                target_match = re.search(r"Target: \[([\d\.-]+), ([\d\.-]+), ([\d\.-]+)\]", line)
                error_match = re.search(r"Error: ([\d\.]+)", line)
                time_match = re.search(r"Elapsed Time: ([\d\.]+)", line)

                target_x = float(target_match.group(1))
                target_y = float(target_match.group(2))
                target_z = float(target_match.group(3))

                # Extraire l'erreur et le temps écoulé
                error = float(error_match.group(1))
                elapsed_time = float(time_match.group(1))

                targets.append((target_x, target_y, target_z))
                error_history.append(error)
                elapsed_times.append(elapsed_time)

        plt.scatter([str(i) for i in targets], [e * 1e3 for e in error_history], c=colors[i])
        plt.title("Error history to target, and elapsed time")
        plt.legend(titles)
        plt.xlabel("Target")
        plt.ylabel("Euclidian distance (mm)")
        for i, t in enumerate(targets):
            plt.annotate(
                f"  n°{i}, {elapsed_times[i]:.2f} s",
                (str(targets[i]), 1e3 * error_history[i]),
            )


def parse_log_file(log_file_path):
    with open(log_file_path) as f:
        reader = csv.reader(f)
        rows_nb = sum(1 for row in reader)
        f.seek(0)
        times = []
        targets = np.empty((rows_nb, 3))
        errors = np.empty((rows_nb, 3))
        croc_torques = np.empty((rows_nb, 5))
        ric_torques = np.empty((rows_nb, 5))
        real_torques = np.empty((rows_nb, 5))
        ddqs = np.empty((rows_nb, 5))

        for i, row in zip(range(rows_nb), reader):
            times.append(float(row[0]))
            targets[i] = [float(x) for x in row[1:4]]
            errors[i] = [float(x) for x in row[4:7]]
            croc_torques[i] = [float(x) for x in row[7:12]]
            ric_torques[i] = [float(x) for x in row[12:17]]
            real_torques[i] = [float(x) for x in row[17:22]]
            ddqs[i] = [float(x) for x in row[22::]]

    return times, targets, errors, croc_torques, ric_torques, real_torques, ddqs


def calculate_jerk(ddqs, times):
    dt = np.diff(times)
    jerk = np.divide(np.diff(ddqs, axis=0), np.array([dt for _ in range(5)]).T)
    return jerk


def main():
    txt_file_names = [
        "error_history_aba.txt",
        "error_history_lin.txt",
        "error_history_none.txt",
    ]
    csv_file_names = [
        "full_history_aba.csv",
        "full_history_lin.csv",
        "full_history_none.csv",
    ]

    plot_error(sys.argv[1], txt_file_names)

    titles = ["ABA", "Linear", "None"]

    for i in range(3):
        (
            times,
            targets,
            errors,
            croc_torques,
            ric_torques,
            real_torques,
            ddqs,
        ) = parse_log_file(sys.argv[1] + csv_file_names[i])
        jerks = calculate_jerk(ddqs, times)

        layout = [
            ["croc_torque", "ric_torque", "real_torque"],
            ["acceleration", "", "jerk"],
        ]

        fig, axd = plt.subplot_mosaic(layout)

        axd["croc_torque"].plot(times, [croc_torques[i, :] for i in range(len(croc_torques))])
        axd["croc_torque"].set_xlabel("Time (s)")
        axd["croc_torque"].set_ylabel("Torque (Nm)")
        axd["croc_torque"].set_title(f"Crocoddyl torque history {titles[i]}")

        axd["ric_torque"].plot(times, [ric_torques[i, :] for i in range(len(ric_torques))])
        axd["ric_torque"].set_xlabel("Time (s)")
        axd["ric_torque"].set_ylabel("Torque (Nm)")
        axd["ric_torque"].set_title(f"Ricatti torque history {titles[i]}")

        axd["real_torques"].plot(times, [real_torques[i, :] for i in range(len(real_torques))])
        axd["real_torques"].set_xlabel("Time (s)")
        axd["real_torques"].set_ylabel("Torque (Nm)")
        axd["real_torques"].set_title(f"Real torque history {titles[i]}")

        axd["acceleration"].plot(times, [ddqs[i, :] for i in range(len(ddqs))])
        axd["acceleration"].set_xlabel("Time (s)")
        axd["acceleration"].set_ylabel("Acceleration (rad/s²)")
        axd["acceleration"].set_title(f"Acceleration history {titles[i]}")

        axd["jerk"].plot(times[:-1], [jerks[i, :] for i in range(len(jerks))])
        axd["jerk"].set_xlabel("Time (s)")
        axd["jerk"].set_ylabel("Jerk (rad/s³)")
        axd["jerk"].set_title(f"Jerk history {titles[i]}")

        plt.tight_layout()

    plt.show()


if __name__ == "__main__":
    main()
