import re
from collections import Counter

def count_word_frequency(file_path):
    with open(file_path, 'r') as file:
        words = re.findall(r'\w+', file.read().lower())

    word_counts = Counter(words)

    sorted_word_counts = sorted(word_counts.items(), key=lambda x: x[1], reverse=True)

    return sorted_word_counts

def write_to_file(sorted_word_counts, output_file):
    with open(output_file, 'w') as file:
        for word, count in sorted_word_counts:
            file.write(f"{word}: {count}\n")

def main():
    input_file = 'path/to/your/input.txt'
    output_file = 'path/to/your/output.txt'

    sorted_word_counts = count_word_frequency(input_file)
    write_to_file(sorted_word_counts, output_file)
    print(f"Word frequency written to {output_file}")

if __name__ == '__main__':
    main()
