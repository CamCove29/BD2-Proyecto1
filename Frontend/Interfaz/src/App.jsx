import React, { useState, useRef, useEffect } from 'react';

const App = () => {
  const [terminalContent, setTerminalContent] = useState([]);
  const [inputValue, setInputValue] = useState('');
  const [availableTables, setAvailableTables] = useState(['CustomerSparse']);
  const [tableData, setTableData] = useState({
    Headers: [],
    Results: []
  });

  const [metadata, setMetadata] = useState({
    headers: [],
    tables: []
  });

  const inputRef = useRef(null);
  const terminalRef = useRef(null);

  useEffect(() => {
    if (terminalRef.current) {
      terminalRef.current.scrollTop = terminalRef.current.scrollHeight;
    }
  }, [terminalContent]);

  useEffect(() => {
    const fetchMetadata = async () => {
      try {
        const response = await fetch('http://127.0.0.1:5000/metadata');
        const data = await response.json();
        setMetadata(data);
      } catch (error) {
        console.error('Error fetching metadata:', error);
      }
    };

    fetchMetadata();
  }, []);

  const handleTerminalClick = () => {
    inputRef.current.focus();
  };

  const handleInputChange = (e) => {
    setInputValue(e.target.value);
  };

  const handleKeyPress = async (e) => {
    if (e.key === 'Enter') {
      const newContent = [...terminalContent, `SQL_parser> ${inputValue}`];
      setTerminalContent(newContent);
      setInputValue('');
      try {
        const response = await getResponse(inputValue);
        const response_separate_by_lines = response.split('\n');
        setTerminalContent([...newContent, ...response_separate_by_lines]);

        if (inputValue.toUpperCase().startsWith('SELECT')) {
          try {
            const headers = response_separate_by_lines[0].split(',');
            const results = response_separate_by_lines.slice(1).filter((cosa) => {
              return cosa !== "";
            }).map((row) => {
              const values = row.split(',');
              const rowObject = {};
              headers.forEach((header, index) => {
                rowObject[header] = values[index];
              });
              return rowObject;
            });

            setTableData({ Headers: headers, Results: results });
          } catch (error) {
            setTerminalContent([...newContent, 'Error: ' + error.message]);
          }
        }

      } catch (error) {
        setTerminalContent([...newContent, 'Error: ' + error.message]);
      }

      if (inputValue === 'clear') {
        setTerminalContent([]);
        setInputValue('');
        setTableData({ Headers: [], Results: [] });
      }
    }
  };

  const getResponse = async (input) => {
    const response = await fetch('http://127.0.0.1:5000/run_query', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({ query: input })
    });

    const data = await response.json();
    const output = data.output;
    return output;
  };

  return (
    <div className="container mx-auto p-4">
      <header className="bg-[#001F3F] text-[#ffffff] p-4 text-center rounded-t-lg shadow-lg mb-5">
        <h1 className="text-2xl font-bold">Nombre de la Aplicación</h1>
      </header>
      <div className="flex gap-4 mt-2">
        <div 
          className="terminal bg-[#001F3F] text-white p-4 rounded-lg flex-grow cursor-text shadow-md" 
          onClick={handleTerminalClick}
          ref={terminalRef}
        >
          <h2 className="text-xl font-bold mb-2">TERMINAL</h2>
          <div className="h-[30vh] overflow-y-auto">
            <div className="whitespace-pre-wrap text-[#FFD700]">
              {terminalContent.map((line, index) => (
                <div key={index}>
                  {line.startsWith('SQL_parser>') ? (
                    <span className="text-white">{line}</span>
                  ) : (
                    line
                  )}
                  <span>&#8203;</span>
                </div>
              ))}
            </div>
            <div className="flex items-center mt-2">
              <span className="text-white">{"SQL_parser>"}</span>
              <span className="w-2"></span>
              <input
                ref={inputRef}
                type="text"
                value={inputValue}
                onChange={handleInputChange}
                onKeyPress={handleKeyPress}
                className="bg-transparent outline-none w-full text-[#FFD700]"
              />
            </div>
          </div>
        </div>
        <div className="tablas bg-[#001F3F] text-white p-4 rounded-lg shadow-md">
          <h2 className="text-xl font-bold mb-2">TABLAS DISPONIBLES</h2>
          {metadata.tables.map((table, index) => (
            <p key={index}>{table[0]}</p>
          ))}
        </div>
      </div>
      {tableData.Results.length === 0 ? (
        <div style={{ overflowX: 'auto', height: '50vh', overflowY: 'auto' }}>
          <table className="w-full mt-4 bg-white border-collapse rounded-lg overflow-hidden"> 
            <thead>
              <tr>
                {metadata.headers.map((header, index) => (
                  <th key={index} className="border border-gray-300 p-2 bg-[#FFD700] text-[#ffffff]">{header}</th>
                ))}
              </tr>
            </thead>
            <tbody>
              {metadata.tables.map((row, rowIndex) => (
                <tr key={rowIndex}>
                  {row.map((cell, cellIndex) => (
                    <td key={cellIndex} className="border border-gray-300 p-2">{cell}</td>
                  ))}
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      ) : (
        <div style={{ overflowX: 'auto', height: '50vh' }}>
          <table className="w-full mt-4 bg-white border-collapse rounded-lg overflow-hidden"> 
            <thead>
              <tr>
                {tableData.Headers.map((header, index) => (
                  <th key={index} className="border border-gray-300 p-2 bg-[#FFD700] text-[#ffffff]">{header}</th>
                ))}
              </tr>
            </thead>
            <tbody>
              {tableData.Results.map((row, rowIndex) => (
                <tr key={rowIndex}>
                  {tableData.Headers.map((header, cellIndex) => (
                    <td key={cellIndex} className="border border-gray-300 p-2">{row[header]}</td>
                  ))}
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
    </div>
  );
};

export default App;
