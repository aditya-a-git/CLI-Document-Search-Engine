# CLI-Document-Search-Engine

A command-line document search engine built in C++ that indexes and searches across multiple documents (TXT, PDF, DOCX) using an inverted index. This project focus on how basic search engines work internally, emphasizing data structures, efficiency, and clean system design.

## Features

- Indexes multiple documents and supports keyword-based search
- Supports TXT, PDF, and DOCX files
- Uses inverted indexing with line-level positional information
- Case-insensitive search with text normalization
- Displays matched lines with document names and line numbers
- Clean CLI-based interface

## Technical Overview

### Indexing Strategy

Built an inverted index using:

```cpp
unordered_map<string, unordered_map<int, vector<int>>>
```

Each word maps to:

- Document ID
- List of line numbers where the word appears

### Text Processing

- Tokenization using string streams
- Normalization:
  - Converts text to lowercase
  - Removes non-alphanumeric characters
  - Handles noisy input from converted PDF/DOCX files

### Search

- Prefix-based keyword search
- Aggregates results across multiple documents
- Displays contextual results with formatted output

## Supported File Types

| File Type | Handling Method          |
|-----------|--------------------------|
| .txt      | Direct indexing          |
| .pdf      | Converted using pdftotext |
| .docx     | Converted using pandoc   |

Temporary files are generated during conversion process and are automatically cleaned up after program ends.

## External Dependencies

The following tools must be installed and available in the system path:

- pdftotext (for PDF conversion)
- pandoc (for DOCX conversion)

If unavailable, the program reports an appropriate error.

## Complexity Analysis

### Indexing Time
O(N), where N is the total number of tokens across all documents

### Search Time
O(V) for prefix search, where V is vocabulary size  
(Can be optimized using Trie-based indexing)

### Space Complexity
O(N) for storing inverted index with positional information

## Possible Improvements

- Replace prefix-based linear scan with Trie-based indexing
- Introduce multithreaded indexing for large document sets

## Author

Aditya A  
B.Tech Computer Science, BMS College of Engineering