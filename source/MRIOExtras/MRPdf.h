#pragma once

#include "config.h"
#ifndef MRIOEXTRAS_NO_PDF
#include "exports.h"
#include "MRMesh/MRMeshFwd.h"
#include "MRMesh/MRVector2.h"
#include "MRMesh/MRBox.h"
#include "MRMesh/MRColor.h"

#include <filesystem>
#include <vector>

namespace MR
{

/**
 * @brief Parameters of document style
 */
struct PdfParameters
{
    float titleSize = 18.f;
    float textSize = 14.f;
    /**
     * @brief Font name
     * list of available fonts:
     * Courier (-Bold, -Oblique, -BoldOblique)
     * Helvetica (-Bold, -Oblique, -BoldOblique)
     * Times (-Roman, -Bold, -Italic, -BoldItalic)
     * Symbol
     * ZapfDingbats
     */
    std::string defaultFontName = "Helvetica";
    /**
    * Font name for table (monospaced)
    */
    std::string tableFontName = "Courier";
};

/**
 * Class for simple creation pdf.
 */
class Pdf
{
public:
    /// Ctor
    MRIOEXTRAS_API Pdf( const PdfParameters& params = PdfParameters() );
    MRIOEXTRAS_API Pdf( Pdf&& other ) noexcept;
    MRIOEXTRAS_API Pdf& operator=( Pdf other ) noexcept; // Sic, passing by value.
    /// Dtor. Automatically do close
    MRIOEXTRAS_API ~Pdf();

    /**
     * Add text block in current cursor position.
     * Move cursor.
     * Box horizontal size is page width without offset.
     * Box vertical size is automatically for text.
     * horAlignment = left
     * if isTitle - horAlignment = center, use titleFontSize
     */
    MRIOEXTRAS_API void addText( const std::string& text, bool isTitle = false );

    /**
     * Add set of pair string - value in current cursor position.
     * Move cursor.
     * Box horizontal size is page width without offset.
     * Box vertical size is automatically for text.
     */
    MRIOEXTRAS_API void addTable( const std::vector<std::pair<std::string, float>>& table );

    struct PaletteRowStats
    {
        Color color;
        std::string rangeMin;
        std::string rangeMax;
        std::string percent;
    };
    MRIOEXTRAS_API void addPaletteStatsTable( const std::vector<PaletteRowStats>& paletteStats );

    /// Parameters to adding image from file
    struct ImageParams
    {
        /// image size in page space
        /// if == {0, 0} - use image size
        /// if .x or .y < 0 use the available page size from the current cursor position (caption size is also accounted for)
        Vector2f size;
        /// caption if not empty - add caption under marks (if exist) or image.
        std::string caption;
        /// set height to keep same scale as width scale
        bool uniformScaleFromWidth = false;
    };
    /**
     * @brief Add image from file in current cursor position.
     * If image bigger than page size, autoscale image to page size.
     * Move cursor.
     */
    MRIOEXTRAS_API void addImageFromFile( const std::filesystem::path& imagePath, const ImageParams& params ); 

    /// Add new pageand move cursor on it
    MRIOEXTRAS_API void newPage();

    /// Save document to file
    MRIOEXTRAS_API void saveToFile( const std::filesystem::path& documentPath );

    void setCursorPosX( float posX ) { cursorX_ = posX; }
    void setCursorPosY( float posY ) { cursorY_ = posY; }
    float getCursorPosX() const { return cursorX_; }
    float getCursorPosY() const { return cursorY_; }

    MRIOEXTRAS_API Vector2f getPageSize() const;
    MRIOEXTRAS_API Box2f getPageWorkArea() const;

    /// Checking the ability to work with a document
    MRIOEXTRAS_API operator bool() const;
    
    /*
    // Table part
    struct Cell {
        using Value = std::variant<int, float, bool, std::string>;
        Value data;

        template<typename T>
        Cell( T value ) : data( value ) {}

        std::string toString( const std::string& fmt_str = "{}" ) const
        {
            return std::visit( [&] ( const auto& val )
            {
                return fmt::format( fmt_str, val );
            }, data );
        }
    };
    MRIOEXTRAS_API void setTableColumnCounts( int columnCount );
    MRIOEXTRAS_API void setTableColumnWidthsAuto();
    MRIOEXTRAS_API void addTableTitles( const std::vector<std::string>& titles );
    MRIOEXTRAS_API void setRowValuesFormat( const std::vector<std::string>& formats );
    MRIOEXTRAS_API void addRow( const std::vector<Cell>& cells );
    */
private:
    struct TextParams;
    // common method for adding different types of text
    void addText_( const std::string& text, const TextParams& params );

    void drawTextRect_( const std::string& text, const Box2f& rect, const TextParams& params );
    void drawRect_( const Box2f& rect, const Color& fillColor, const Color& strokeColor );

    struct TextCellParams;
    void drawTextCell_( const std::string& text, const TextCellParams& params );


    // close pdf document without saving. After this impossible add anything in document.
    void reset_();

    // count the number of rows with auto-transfer in mind for a given page (page, font and font size)
    int calcTextLinesCount_( const std::string& text );

    struct State;
    std::unique_ptr<State> state_;

    PdfParameters params_;

    float cursorX_ = 0;
    float cursorY_ = 0;

    bool checkDocument_( const std::string& action ) const;
    void moveCursorToNewLine();

    // table parts
    int columnCount_ = 1;
    std::vector<float> columnWidths = { 100 };
    std::vector<std::string> formats_ = { "{}" };

    void drawTableCell_( const std::string& text );


    //std::vector<std::string> formats;
    //std::vector<std::vector<std::string>> rows;

    //// end recursive
    //void formatArgs(std::vector<std::string>&, size_t) {}

    //// recursive format
    //template<typename T, typename... Rest>
    //void formatArgs( std::vector<std::string>& out, size_t index, T&& value, Rest&&... rest )
    //{
    //    std::string fmt = ( index < formats.size() ) ? formats[index] : "{}";
    //    out.push_back( fmt::format( fmt, std::forward<T>( value ) ) );
    //    formatArgs( out, index + 1, std::forward<Rest>( rest )... );
    //}
};

}
#endif
